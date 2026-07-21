#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "globals.h"

class FirebaseManager {
public:
  static void begin() {
    deviceId = WiFi.macAddress();
    deviceId.replace(":", "");

    apiKey = config.str["FIREBASE_API_KEY"];
    databaseUrl = config.str["FIREBASE_DATABASE_URL"];
    deviceEmail = config.str["FIREBASE_DEVICE_EMAIL"];
    devicePassword = config.str["FIREBASE_DEVICE_PASSWORD"];
  }

  static void pushStatus(int moisturePercentage, const String& relayState) {
    if (WiFi.status() != WL_CONNECTED) return;

    unsigned long currentMillis = millis();
    if (currentMillis - lastPush < (unsigned long)config["FIREBASE_PUSH_INTERVAL"]) return;
    lastPush = currentMillis;

    if (!ensureAuthenticated()) {
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

    if (responseCode > 0) {
      Serial.println("Firebase push OK (" + String(responseCode) + ")");
    } else {
      Serial.println("Firebase push failed: " + http.errorToString(responseCode));
    }

    http.end();
  }

private:
  static String deviceId;
  static String idToken;
  static String refreshToken;
  static unsigned long tokenExpiresAt;
  static unsigned long lastPush;

  static String apiKey;
  static String databaseUrl;
  static String deviceEmail;
  static String devicePassword;

  static bool ensureAuthenticated() {
    unsigned long currentMillis = millis();

    if (idToken.length() > 0 && currentMillis < tokenExpiresAt) {
      return true;
    }

    if (refreshToken.length() > 0 && refreshIdToken()) {
      return true;
    }

    return signIn();
  }

  static bool signIn() {
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

    if (responseCode == 200) {
      String response = http.getString();
      success = extractValue(response, "\"idToken\":\"", idToken)
             && extractValue(response, "\"refreshToken\":\"", refreshToken);
      if (success) {
        tokenExpiresAt = millis() + 3000UL * 1000UL;
        Serial.println("Firebase: signed in.");
      }
    } else {
      Serial.println("Firebase sign-in failed: " + String(responseCode));
    }

    http.end();
    return success;
  }

  static bool refreshIdToken() {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    String url = String("https://securetoken.googleapis.com/v1/token?key=") + apiKey;
    String payload = "grant_type=refresh_token&refresh_token=" + refreshToken;

    http.begin(client, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int responseCode = http.POST(payload);
    bool success = false;

    if (responseCode == 200) {
      String response = http.getString();
      success = extractValue(response, "\"id_token\":\"", idToken)
             && extractValue(response, "\"refresh_token\":\"", refreshToken);
      if (success) {
        tokenExpiresAt = millis() + 3000UL * 1000UL;
        Serial.println("Firebase: token refreshed.");
      }
    }

    http.end();
    return success;
  }

  static bool extractValue(const String& json, const char* key, String& out) {
    int start = json.indexOf(key);
    if (start == -1) return false;
    start += strlen(key);
    int end = json.indexOf('"', start);
    if (end == -1) return false;
    out = json.substring(start, end);
    return true;
  }
};

#endif // FIREBASE_MANAGER_H