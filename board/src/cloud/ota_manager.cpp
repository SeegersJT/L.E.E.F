#include "cloud/ota_manager.h"
#include "cloud/firebase_service.h"
#include "core/globals.h"
#include "core/time_utils.h"
#include "core/logger.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

String OtaManager::apiKey = "";
String OtaManager::deviceEmail = "";
String OtaManager::devicePassword = "";
String OtaManager::idToken = "";
String OtaManager::refreshToken = "";
unsigned long OtaManager::tokenExpiresAt = 0;

unsigned long OtaManager::lastOtaCheck = 0;
String OtaManager::lastOtaResult = "";
String OtaManager::lastOtaCheckTime = "";

void OtaManager::begin(const String &apiKeyIn, const String &deviceEmailIn, const String &devicePasswordIn)
{
    apiKey = apiKeyIn;
    deviceEmail = deviceEmailIn;
    devicePassword = devicePasswordIn;
}

const String &OtaManager::lastResult()
{
    return lastOtaResult;
}

const String &OtaManager::lastCheckTime()
{
    return lastOtaCheckTime;
}

void OtaManager::checkForUpdate()
{
    if (WiFi.status() != WL_CONNECTED || !FirebaseService::ready())
        return;

    unsigned long currentMillis = millis();
    if (lastOtaCheck != 0 && currentMillis - lastOtaCheck < (unsigned long)config["OTA_CHECK_INTERVAL"])
        return;
    lastOtaCheck = currentMillis;
    lastOtaCheckTime = currentIsoTimestamp();

    String response;
    if (!FirebaseService::get("/firmware", response))
    {
        lastOtaResult = "RTDB check failed: " + FirebaseService::client().lastError().message();
        Logger::log(LogCategory::LOG_OTA, lastOtaResult);
        return;
    }

    response.trim();
    if (response == "null" || response.length() == 0)
    {
        lastOtaResult = "No firmware entries found";
        Logger::log(LogCategory::LOG_OTA, lastOtaResult);
        return;
    }

    // Walk every entry in /firmware and keep the one with the
    // lexicographically-latest dateUploaded (ISO 8601 sorts correctly as
    // plain text). Downloads the whole small list rather than using a
    // server-side orderBy/limitToLast query - the filtered-query path
    // wasn't reliable in practice, and this list is small enough that
    // fetching it in full costs nothing meaningful.
    String latestKey, latestDate, latestFilename, latestUrl;
    int pos = 0;

    while (true)
    {
        int keyStart = response.indexOf("\"", pos);
        if (keyStart == -1)
            break;
        int keyEnd = response.indexOf("\"", keyStart + 1);
        if (keyEnd == -1)
            break;
        String key = response.substring(keyStart + 1, keyEnd);

        int objStart = response.indexOf("{", keyEnd);
        if (objStart == -1)
            break;
        int objEnd = findMatchingBrace(response, objStart);
        if (objEnd == -1)
            break;

        String entryJson = response.substring(objStart, objEnd + 1);

        String date, filename, url;
        if (extractJsonValue(entryJson, "dateUploaded", date) &&
            extractJsonValue(entryJson, "filename", filename))
        {
            if (date > latestDate)
            {
                latestDate = date;
                latestKey = key;
                latestFilename = filename;
                extractJsonValue(entryJson, "url", url);
                latestUrl = url;
            }
        }

        pos = objEnd + 1;
    }

    if (latestKey.length() == 0)
    {
        lastOtaResult = "No firmware entries found";
        Logger::log(LogCategory::LOG_OTA, lastOtaResult);
        return;
    }

    mirrorFirmwareEntry(latestKey, latestDate, latestFilename, latestUrl);

    if (latestDate == config.str["APPLIED_FIRMWARE_DATE"])
    {
        lastOtaResult = "up-to-date";
        return;
    }

    Logger::log(LogCategory::LOG_OTA, "New firmware available (" + latestDate + "), downloading");
    display("Firmware update").clear().print();
    display("found - flashing").bottom().print();

    if (applyFirmware(latestFilename))
    {
        config.writeString("/config/ota_state.ini", "APPLIED_FIRMWARE_DATE", latestDate);
        display("Update complete").clear().print();
        display("Restarting...").bottom().print();
        delay(2000);
        ESP.restart();
    }
    else
    {
        lastOtaResult = "OTA download/flash failed";
        display("Update failed").clear().print();
        display("Will retry later").bottom().print();
        delay(2000);
    }
}

void OtaManager::mirrorFirmwareEntry(const String &key, const String &date, const String &filename, const String &url)
{
    String path = "/devices/" + FirebaseService::deviceId() + "/config/firmware/" + key;

    String payload = "{";
    payload += "\"dateUploaded\":\"" + date + "\",";
    payload += "\"filename\":\"" + filename + "\",";
    payload += "\"url\":\"" + url + "\"";
    payload += "}";

    if (!FirebaseService::put(path, payload))
    {
        Logger::log(LogCategory::LOG_OTA, "Firmware entry mirror failed");
    }
}

bool OtaManager::applyFirmware(const String &filename)
{
    if (!ensureAuthenticated())
    {
        Logger::log(LogCategory::LOG_OTA, "Not authenticated for Storage download");
        return false;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String encodedPath = "firmware%2F" + filename;
    String url = "https://firebasestorage.googleapis.com/v0/b/" + FirebaseService::storageBucketName() + "/o/" + encodedPath + "?alt=media";

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + idToken);
    int responseCode = http.GET();

    if (responseCode != 200)
    {
        Logger::log(LogCategory::LOG_OTA, "Download failed: " + String(responseCode));
        http.end();
        return false;
    }

    int contentLength = http.getSize();
    WiFiClient *stream = http.getStreamPtr();

    if (contentLength <= 0)
    {
        Logger::log(LogCategory::LOG_OTA, "Unknown content length, aborting download");
        http.end();
        return false;
    }

    if (!Update.begin(contentLength))
    {
        Logger::log(LogCategory::LOG_OTA, "Not enough space for update (" + String(contentLength) + " bytes)");
        http.end();
        return false;
    }

    size_t written = Update.writeStream(*stream);
    http.end();

    if (written != (size_t)contentLength)
    {
        Logger::log(LogCategory::LOG_OTA, "Write incomplete: " + String(written) + "/" + String(contentLength));
        Update.abort();
        return false;
    }

    if (!Update.end(true))
    {
        Logger::log(LogCategory::LOG_OTA, "Finalize failed, error code: " + String(Update.getError()));
        return false;
    }

    return true;
}

bool OtaManager::ensureAuthenticated()
{
    unsigned long currentMillis = millis();

    if (idToken.length() > 0 && currentMillis < tokenExpiresAt)
    {
        return true;
    }

    if (refreshToken.length() > 0 && refreshIdToken())
    {
        return true;
    }

    return signIn();
}

bool OtaManager::signIn()
{
    for (int attempt = 0; attempt < 3; attempt++)
    {
        WiFiClientSecure client;
        client.setInsecure();
        client.setHandshakeTimeout(30);

        HTTPClient http;
        String url = String("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=") + apiKey;

        String payload = "{";
        payload += "\"email\":\"" + deviceEmail + "\",";
        payload += "\"password\":\"" + devicePassword + "\",";
        payload += "\"returnSecureToken\":true";
        payload += "}";

        http.begin(client, url);
        http.addHeader("Content-Type", "application/json");
        int responseCode = http.POST(payload);

        if (responseCode == 200)
        {
            String response = http.getString();

            bool success = extractJsonValue(response, "idToken", idToken) && extractJsonValue(response, "refreshToken", refreshToken);

            http.end();

            if (success)
            {
                tokenExpiresAt = millis() + 3000UL * 1000UL;
                Logger::log(LogCategory::LOG_OTA, "Signed in for Storage access");
                return true;
            }
        }
        else
        {
            Logger::log(LogCategory::LOG_OTA, "Storage sign-in attempt " + String(attempt + 1) + " failed: " + String(responseCode));
        }

        http.end();
        delay(1000);
    }

    return false;
}

bool OtaManager::refreshIdToken()
{
    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String url = String("https://securetoken.googleapis.com/v1/token?key=") + apiKey;
    String payload = "grant_type=refresh_token&refresh_token=" + refreshToken;

    http.begin(client, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int responseCode = http.POST(payload);
    bool success = false;

    if (responseCode == 200)
    {
        String response = http.getString();
        success = extractJsonValue(response, "id_token", idToken) && extractJsonValue(response, "refresh_token", refreshToken);
        if (success)
        {
            tokenExpiresAt = millis() + 3000UL * 1000UL;
            Logger::log(LogCategory::LOG_OTA, "Storage token refreshed");
        }
    }

    http.end();
    return success;
}

bool OtaManager::extractJsonValue(const String &json, const char *fieldName, String &out)
{
    String searchKey = String("\"") + fieldName + "\"";
    int start = json.indexOf(searchKey);
    if (start == -1)
        return false;
    start += searchKey.length();

    while (start < (int)json.length() && (json[start] == ' ' || json[start] == ':'))
        start++;

    if (start >= (int)json.length() || json[start] != '"')
        return false;
    start++;

    int end = json.indexOf('"', start);
    if (end == -1)
        return false;
    out = json.substring(start, end);
    return true;
}

int OtaManager::findMatchingBrace(const String &text, int openIndex)
{
    int depth = 0;

    for (int i = openIndex; i < (int)text.length(); i++)
    {
        if (text[i] == '{')
        {
            depth++;
        }
        else if (text[i] == '}')
        {
            depth--;
            if (depth == 0)
            {
                return i;
            }
        }
    }
    return -1;
}