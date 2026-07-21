#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include "globals.h"

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

  static void pushStatus(int moisturePercentage, const String &relayState)
  {
    if (WiFi.status() != WL_CONNECTED)
      return;

    unsigned long currentMillis = millis();
    if (currentMillis - lastPush < (unsigned long)config["FIREBASE_PUSH_INTERVAL"])
      return;
    lastPush = currentMillis;

    if (!ensureAuthenticated())
    {
      Serial.println("Firebase: not authenticated, skipping push.");
      return;
    }

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    String url = databaseUrl + "/devices/" + deviceId + "/status.json?auth=" + idToken;

    String payload = "{";
    payload += "\"moisture\":" + String(moisturePercentage) + ",";
    payload += "\"relayState\":\"" + relayState + "\",";
    payload += "\"lastSeen\":" + String(currentMillis);
    payload += "}";

    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    int responseCode = http.PUT(payload);

    if (responseCode > 0)
    {
      Serial.println("Firebase push OK (" + String(responseCode) + ")");
    }
    else
    {
      Serial.println("Firebase push failed: " + http.errorToString(responseCode));
    }

    http.end();
  }

  static void checkForFirmwareUpdate()
  {
    if (WiFi.status() != WL_CONNECTED)
      return;

    unsigned long currentMillis = millis();
    if (currentMillis - lastOtaCheck < (unsigned long)config["OTA_CHECK_INTERVAL"])
      return;
    lastOtaCheck = currentMillis;

    if (!ensureAuthenticated())
    {
      Serial.println("Firebase: not authenticated, skipping OTA check.");
      return;
    }

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    String url = databaseUrl + "/firmware.json?auth=" + idToken + "&orderBy=%22dateUploaded%22&limitToLast=1";

    http.begin(client, url);
    int responseCode = http.GET();

    if (responseCode != 200)
    {
      Serial.println("OTA check failed: " + String(responseCode));
      http.end();
      return;
    }

    String response = http.getString();
    http.end();

    String latestDate, latestFilename;
    if (!extractValue(response, "\"dateUploaded\":\"", latestDate) ||
        !extractValue(response, "\"filename\":\"", latestFilename))
    {
      Serial.println("OTA check: no firmware entries found yet.");
      return;
    }

    if (latestDate == config.str["APPLIED_FIRMWARE_DATE"])
    {
      return;
    }

    Serial.println("New firmware available (" + latestDate + "), downloading...");
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
      display("Update failed").clear().print();
      display("Will retry later").bottom().print();
      delay(2000);
    }
  }

private:
  static String deviceId;
  static String idToken;
  static String refreshToken;
  static unsigned long tokenExpiresAt;
  static unsigned long lastPush;
  static unsigned long lastOtaCheck;

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
    WiFiClientSecure client;
    client.setInsecure();

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
    bool success = false;

    if (responseCode == 200)
    {
      String response = http.getString();
      success = extractValue(response, "\"idToken\":\"", idToken) && extractValue(response, "\"refreshToken\":\"", refreshToken);
      if (success)
      {
        tokenExpiresAt = millis() + 3000UL * 1000UL;
        Serial.println("Firebase: signed in.");
      }
    }
    else
    {
      Serial.println("Firebase sign-in failed: " + String(responseCode));
    }

    http.end();
    return success;
  }

  static bool refreshIdToken()
  {
    WiFiClientSecure client;
    client.setInsecure();

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
        Serial.println("Firebase: token refreshed.");
      }
    }

    http.end();
    return success;
  }

  static bool extractValue(const String &json, const char *key, String &out)
  {
    int start = json.indexOf(key);
    if (start == -1)
      return false;
    start += strlen(key);
    int end = json.indexOf('"', start);
    if (end == -1)
      return false;
    out = json.substring(start, end);
    return true;
  }

  static bool applyFirmware(const String &filename)
  {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    String encodedPath = "firmware%2F" + filename;
    String url = "https://firebasestorage.googleapis.com/v0/b/" + storageBucket + "/o/" + encodedPath + "?alt=media";

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + idToken);
    int responseCode = http.GET();

    if (responseCode != 200)
    {
      Serial.println("OTA download failed: " + String(responseCode));
      http.end();
      return false;
    }

    int contentLength = http.getSize();
    WiFiClient *stream = http.getStreamPtr();

    if (contentLength <= 0)
    {
      Serial.println("OTA download: unknown content length, aborting.");
      http.end();
      return false;
    }

    if (!Update.begin(contentLength))
    {
      Serial.println("OTA: not enough space for update (" + String(contentLength) + " bytes).");
      http.end();
      return false;
    }

    size_t written = Update.writeStream(*stream);
    http.end();

    if (written != (size_t)contentLength)
    {
      Serial.println("OTA write incomplete: " + String(written) + "/" + String(contentLength));
      Update.abort();
      return false;
    }

    if (!Update.end(true))
    {
      Serial.println("OTA finalize failed, error code: " + String(Update.getError()));
      return false;
    }

    return true;
  }
};

#endif // FIREBASE_MANAGER_H