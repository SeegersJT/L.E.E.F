#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include <esp_system.h>
#include "globals.h"
#include "time_utils.h"
#include "logger.h"

class FirebaseManager
{
public:
  static void begin()
  {
    deviceId = WiFi.macAddress();
    deviceId.replace(":", "");

    apiKey = config.str["FIREBASE_API_KEY"];
    databaseUrl = config.str["FIREBASE_DATABASE_URL"];
    deviceEmail = config.str["FIREBASE_DEVICE_EMAIL"];
    devicePassword = config.str["FIREBASE_DEVICE_PASSWORD"];
    storageBucket = config.str["FIREBASE_STORAGE_BUCKET"];
  }

  static void pushStatus(int moisturePercentage, const String &moistureTimestamp,
                         const String &relayState, const String &relayTimestamp)
  {
    if (WiFi.status() != WL_CONNECTED)
      return;

    unsigned long currentMillis = millis();
    if (lastPush != 0 && currentMillis - lastPush < (unsigned long)config["FIREBASE_PUSH_INTERVAL"])
      return;
    lastPush = currentMillis;

    if (!ensureAuthenticated())
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Not authenticated, skipping push");
      return;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String url = databaseUrl + "/devices/" + deviceId + "/status.json?auth=" + idToken;

    String payload = buildStatusPayload(moisturePercentage, moistureTimestamp, relayState, relayTimestamp);

    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    int responseCode = http.PUT(payload);

    if (responseCode > 0)
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Push OK (" + String(responseCode) + ")");
    }
    else
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Push failed: " + http.errorToString(responseCode));
    }

    http.end();
  }

  static void logMoistureReading(int moisturePercentage, const String &timestamp)
  {
    if (timestamp.length() == 0 || WiFi.status() != WL_CONNECTED)
      return;

    if (!ensureAuthenticated())
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Not authenticated, skipping moisture history entry");
      return;
    }

    String payload = "{";
    payload += "\"reading\":" + String(moisturePercentage) + ",";
    payload += "\"unit\":\"percent\"";
    payload += "}";

    putHistoryEntry("MOISTURE_SENSOR_PIN_MM01", timestamp, payload);
  }

  static void logRelayEvent(const String &state, const String &timestamp)
  {
    if (timestamp.length() == 0 || WiFi.status() != WL_CONNECTED)
      return;

    if (!ensureAuthenticated())
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Not authenticated, skipping relay history entry");
      return;
    }

    String payload = "{";
    payload += "\"state\":\"" + state + "\"";
    payload += "}";

    putHistoryEntry("RELAY_PIN_R01", timestamp, payload);
  }

  static void maintainPairing()
  {
    if (WiFi.status() != WL_CONNECTED)
      return;

    unsigned long currentMillis = millis();

    if (deviceClaimed)
    {
      if (lastClaimedRecheck == 0 || currentMillis - lastClaimedRecheck >= (unsigned long)config["OWNERSHIP_RECHECK_INTERVAL"])
      {
        lastClaimedRecheck = currentMillis;
        recheckClaimedStatus();
      }
      return;
    }

    if (lastOwnershipCheck == 0 || currentMillis - lastOwnershipCheck >= (unsigned long)config["PAIRING_CHECK_INTERVAL"])
    {
      lastOwnershipCheck = currentMillis;
      refreshOwnershipStatus();
    }

    if (deviceClaimed || pairingCode.length() == 0)
      return;

    if (currentMillis - lastPairingToggle >= 3000)
    {
      lastPairingToggle = currentMillis;
      showingPairingCode = !showingPairingCode;

      if (showingPairingCode)
      {
        display("Pairing code:").clear().print();
        display(pairingCode).bottom().print();
      }
      else
      {
        display("Enter in app").clear().print();
        display("to link plant").bottom().print();
      }
    }
  }

  static void checkForFirmwareUpdate()
  {
    if (WiFi.status() != WL_CONNECTED)
      return;

    unsigned long currentMillis = millis();
    if (lastOtaCheck != 0 && currentMillis - lastOtaCheck < (unsigned long)config["OTA_CHECK_INTERVAL"])
      return;
    lastOtaCheck = currentMillis;
    lastOtaCheckTime = currentIsoTimestamp();

    if (!ensureAuthenticated())
    {
      Logger::log(LogCategory::LOG_OTA, "Not authenticated, skipping OTA check");
      return;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String url = databaseUrl + "/firmware.json?auth=" + idToken + "&orderBy=%22dateUploaded%22&limitToLast=1";

    http.begin(client, url);
    int responseCode = http.GET();

    if (responseCode != 200)
    {
      Logger::log(LogCategory::LOG_OTA, "Firmware check failed: " + String(responseCode));
      lastOtaResult = "RTDB check failed (" + String(responseCode) + ")";
      http.end();
      return;
    }

    String response = http.getString();
    http.end();

    String latestKey, latestDate, latestFilename, latestUrl;

    int keyStart = response.indexOf("{\"") + 2;
    int keyEnd = response.indexOf("\":{", keyStart);
    if (keyEnd > keyStart)
    {
      latestKey = response.substring(keyStart, keyEnd);
    }

    if (!extractValue(response, "dateUploaded", latestDate) ||
        !extractValue(response, "filename", latestFilename))
    {
      Logger::log(LogCategory::LOG_OTA, "No firmware entries found yet");
      return;
    }

    extractValue(response, "url", latestUrl);

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

private:
  static String buildStatusPayload(int moisturePercentage, const String &moistureTimestamp, const String &relayState, const String &relayTimestamp)
  {
    String payload = "{";

    payload += "\"MOISTURE_SENSOR_PIN_MM01\":{";
    payload += "\"label\":\"MOISTURE SENSOR 01\",";
    payload += "\"pin\":" + String((int)config["MOISTURE_SENSOR_PIN_MM01"]) + ",";
    payload += "\"reading\":" + String(moisturePercentage) + ",";
    payload += "\"unit\":\"percent\",";
    payload += "\"timestamp\":\"" + moistureTimestamp + "\",";
    payload += "\"intervalMinutes\":" + String((int)config["MOISTURE_SENSORS_INTERVAL_MINUTES"]);
    payload += "},";

    payload += "\"RELAY_PIN_R01\":{";
    payload += "\"label\":\"RELAY 01\",";
    payload += "\"pin\":" + String((int)config["RELAY_PIN_R01"]) + ",";
    payload += "\"state\":\"" + relayState + "\",";
    payload += "\"timestamp\":\"" + relayTimestamp + "\",";
    payload += "\"onDurationMs\":" + String((int)config["RELAY_ON_DURATION"]);
    payload += "},";

    payload += "\"appliedFirmwareDate\":\"" + config.str["APPLIED_FIRMWARE_DATE"] + "\",";
    payload += "\"lastOtaResult\":\"" + lastOtaResult + "\",";
    payload += "\"lastOtaCheckTime\":\"" + lastOtaCheckTime + "\",";
    payload += "\"lastSeen\":\"" + currentIsoTimestamp() + "\"";
    payload += "}";

    return payload;
  }

  static void putHistoryEntry(const String &deviceKey, const String &timestamp, const String &payload)
  {
    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String url = databaseUrl + "/devices/" + deviceId + "/history/" + deviceKey + "/" +
                 urlEncodePathSegment(timestamp) + ".json?auth=" + idToken;

    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    int responseCode = http.PUT(payload);

    if (responseCode > 0)
    {
      Logger::log(LogCategory::LOG_FIREBASE, "History entry logged: " + deviceKey);
    }
    else
    {
      Logger::log(LogCategory::LOG_FIREBASE, "History entry failed: " + http.errorToString(responseCode));
    }

    http.end();
  }

  static void refreshOwnershipStatus()
  {
    if (!ensureAuthenticated())
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Not authenticated, skipping ownership check");
      return;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String url = databaseUrl + "/devices/" + deviceId + "/owner.json?auth=" + idToken;

    http.begin(client, url);
    int responseCode = http.GET();

    if (responseCode != 200)
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Ownership check failed: " + String(responseCode));
      http.end();
      return;
    }

    String response = http.getString();
    http.end();
    response.trim();

    if (response == "null" || response.length() == 0)
    {
      if (pairingCode.length() == 0)
      {
        pairingCode = generatePairingCode();
        publishPairingCode();
      }
      return;
    }

    deviceClaimed = true;
    Logger::log(LogCategory::LOG_FIREBASE, "Device claimed by a user account");
    clearPairingCode();

    display("Paired!").clear().print();
    display("Setup complete").bottom().print();
    delay(2000);
  }

  static void recheckClaimedStatus()
  {
    if (!ensureAuthenticated())
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Not authenticated, skipping ownership recheck");
      return;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String url = databaseUrl + "/devices/" + deviceId + "/owner.json?auth=" + idToken;

    http.begin(client, url);
    int responseCode = http.GET();

    if (responseCode != 200)
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Ownership recheck failed: " + String(responseCode));
      http.end();
      return;
    }

    String response = http.getString();
    http.end();
    response.trim();

    if (response == "null" || response.length() == 0)
    {
      deviceClaimed = false;
      pairingCode = "";
      lastOwnershipCheck = 0;
      Logger::log(LogCategory::LOG_FIREBASE, "Device was unpaired - resuming pairing mode");

      display("Unpaired").clear().print();
      display("Ready to re-pair").bottom().print();
      delay(2000);
    }
  }

  static String generatePairingCode()
  {
    static const char charset[] = "ABCDEFGHJKMNPQRSTUVWXYZ23456789";
    const int codeLength = 6;

    randomSeed(esp_random());

    String code = "";
    for (int i = 0; i < codeLength; i++)
    {
      code += charset[random(0, (int)sizeof(charset) - 1)];
    }

    return code;
  }

  static void publishPairingCode()
  {
    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String url = databaseUrl + "/devices/" + deviceId + "/pairingCode.json?auth=" + idToken;

    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    int responseCode = http.PUT("\"" + pairingCode + "\"");

    if (responseCode > 0)
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Pairing code published");
    }
    else
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Pairing code publish failed: " + http.errorToString(responseCode));
    }

    http.end();

    WiFiClientSecure indexClient;
    indexClient.setInsecure();
    indexClient.setHandshakeTimeout(30);

    HTTPClient indexHttp;
    String indexUrl = databaseUrl + "/pairingCodes/" + pairingCode + ".json?auth=" + idToken;

    String indexPayload = "{";
    indexPayload += "\"deviceId\":\"" + deviceId + "\",";
    indexPayload += "\"createdAt\":{\".sv\":\"timestamp\"}";
    indexPayload += "}";

    indexHttp.begin(indexClient, indexUrl);
    indexHttp.addHeader("Content-Type", "application/json");
    int indexResponseCode = indexHttp.PUT(indexPayload);

    if (indexResponseCode > 0)
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Pairing code indexed");
    }
    else
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Pairing code index failed: " + indexHttp.errorToString(indexResponseCode));
    }

    indexHttp.end();
  }

  static void clearPairingCode()
  {
    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String url = databaseUrl + "/devices/" + deviceId + "/pairingCode.json?auth=" + idToken;

    http.begin(client, url);
    int responseCode = http.sendRequest("DELETE");

    if (responseCode <= 0)
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Pairing code cleanup failed: " + http.errorToString(responseCode));
    }

    http.end();

    WiFiClientSecure indexClient;
    indexClient.setInsecure();
    indexClient.setHandshakeTimeout(30);

    HTTPClient indexHttp;
    String indexUrl = databaseUrl + "/pairingCodes/" + pairingCode + ".json?auth=" + idToken;

    indexHttp.begin(indexClient, indexUrl);
    int indexResponseCode = indexHttp.sendRequest("DELETE");

    if (indexResponseCode <= 0)
    {
      Logger::log(LogCategory::LOG_FIREBASE, "Pairing code index cleanup failed: " + indexHttp.errorToString(indexResponseCode));
    }

    indexHttp.end();
  }

  static String deviceId;
  static String idToken;
  static String refreshToken;
  static unsigned long tokenExpiresAt;
  static unsigned long lastPush;
  static unsigned long lastOtaCheck;
  static String lastOtaResult;
  static String lastOtaCheckTime;

  static String pairingCode;
  static bool deviceClaimed;
  static unsigned long lastOwnershipCheck;
  static unsigned long lastPairingToggle;
  static bool showingPairingCode;
  static unsigned long lastClaimedRecheck;

  static String apiKey;
  static String databaseUrl;
  static String deviceEmail;
  static String devicePassword;
  static String storageBucket;

  static bool ensureAuthenticated()
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

  static bool signIn()
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

        bool success = extractValue(response, "idToken", idToken) && extractValue(response, "refreshToken", refreshToken);

        http.end();

        if (success)
        {
          tokenExpiresAt = millis() + 3000UL * 1000UL;
          Logger::log(LogCategory::LOG_FIREBASE, "Signed in");
          return true;
        }
      }
      else
      {
        Logger::log(LogCategory::LOG_FIREBASE, "Sign-in attempt " + String(attempt + 1) + " failed: " + String(responseCode));
      }

      http.end();
      delay(1000);
    }

    return false;
  }

  static bool refreshIdToken()
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
      success = extractValue(response, "\"id_token\":\"", idToken) && extractValue(response, "\"refresh_token\":\"", refreshToken);
      if (success)
      {
        tokenExpiresAt = millis() + 3000UL * 1000UL;
        Logger::log(LogCategory::LOG_FIREBASE, "Token refreshed");
      }
    }

    http.end();
    return success;
  }

  static bool extractValue(const String &json, const char *fieldName, String &out)
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

  static String urlEncodePathSegment(const String &value)
  {
    String encoded = value;
    encoded.replace(" ", "%20");
    encoded.replace(":", "%3A");
    return encoded;
  }

  static void mirrorFirmwareEntry(const String &key, const String &date, const String &filename, const String &url)
  {
    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String path = databaseUrl + "/devices/" + deviceId + "/config/firmware/" + urlEncodePathSegment(key) + ".json?auth=" + idToken;

    String payload = "{";
    payload += "\"dateUploaded\":\"" + date + "\",";
    payload += "\"filename\":\"" + filename + "\",";
    payload += "\"url\":\"" + url + "\"";
    payload += "}";

    http.begin(client, path);
    http.addHeader("Content-Type", "application/json");
    int responseCode = http.PUT(payload);

    if (responseCode <= 0)
    {
      Logger::log(LogCategory::LOG_OTA, "Firmware entry mirror failed: " + http.errorToString(responseCode));
    }

    http.end();
  }

  static bool applyFirmware(const String &filename)
  {
    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    String encodedPath = "firmware%2F" + filename;
    String url = "https://firebasestorage.googleapis.com/v0/b/" + storageBucket + "/o/" + encodedPath + "?alt=media";

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
};

#endif // FIREBASE_MANAGER_H