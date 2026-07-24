#include "cloud/ota_manager.h"
#include "cloud/firebase_client.h"
#include "core/globals.h"
#include "core/time_utils.h"
#include "core/logger.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

unsigned long OtaManager::lastOtaCheck = 0;
String OtaManager::lastOtaResult = "";
String OtaManager::lastOtaCheckTime = "";

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
    if (WiFi.status() != WL_CONNECTED)
        return;

    unsigned long currentMillis = millis();
    if (lastOtaCheck != 0 && currentMillis - lastOtaCheck < (unsigned long)config["OTA_CHECK_INTERVAL"])
        return;
    lastOtaCheck = currentMillis;
    lastOtaCheckTime = currentIsoTimestamp();

    String response;
    String path = "/firmware";
    String extraQuery = "&orderBy=%22dateUploaded%22&limitToLast=1";

    if (!FirebaseClient::get(path, response, extraQuery))
    {
        Logger::log(LogCategory::LOG_OTA, "Firmware check failed");
        lastOtaResult = "RTDB check failed";
        return;
    }

    String latestKey, latestDate, latestFilename, latestUrl;

    int keyStart = response.indexOf("{\"") + 2;
    int keyEnd = response.indexOf("\":{", keyStart);
    if (keyEnd > keyStart)
    {
        latestKey = response.substring(keyStart, keyEnd);
    }

    if (!FirebaseClient::extractJsonValue(response, "dateUploaded", latestDate) ||
        !FirebaseClient::extractJsonValue(response, "filename", latestFilename))
    {
        Logger::log(LogCategory::LOG_OTA, "No firmware entries found yet");
        return;
    }

    FirebaseClient::extractJsonValue(response, "url", latestUrl);

    if (latestKey.length() > 0)
    {
        mirrorFirmwareEntry(latestKey, latestDate, latestFilename, latestUrl);
    }

    if (latestDate == config.str["APPLIED_FIRMWARE_DATE"])
    {
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
    String path = "/devices/" + FirebaseClient::deviceId() + "/config/firmware/" + FirebaseClient::urlEncodePathSegment(key);

    String payload = "{";
    payload += "\"dateUploaded\":\"" + date + "\",";
    payload += "\"filename\":\"" + filename + "\",";
    payload += "\"url\":\"" + url + "\"";
    payload += "}";

    if (!FirebaseClient::put(path, payload))
    {
        Logger::log(LogCategory::LOG_OTA, "Firmware entry mirror failed");
    }
}

bool OtaManager::applyFirmware(const String &filename)
{
    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String encodedPath = "firmware%2F" + filename;
    String url = "https://firebasestorage.googleapis.com/v0/b/" + FirebaseClient::storageBucketName() + "/o/" + encodedPath + "?alt=media";

    http.begin(client, url);
    http.addHeader("Authorization", FirebaseClient::authorizationHeaderValue());
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