#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "globals.h"

class WiFiManager {
public:
  static bool connectToWiFi(int maxRetries = config["WIFI_CONNECT_MAX_RETRIES"], unsigned long retryDelay = config["WIFI_CONNECT_RETRY_DELAY"]) {
    display("Connecting WiFi").clear().print();

    const char* WIFI_SSID = config.str["WIFI_SSID"].c_str();
    const char* WIFI_PASSWORD = config.str["WIFI_PASSWORD"].c_str();

    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
      delay(retryDelay);
      retries++;
      display("Connecting... " + String(retries) + "/" + String(maxRetries)).bottom().print();
    }

    if (WiFi.status() == WL_CONNECTED) {
      display("Connected").clear().print();
      display(WiFi.localIP().toString()).bottom().print();
      delay(2000);
      return true;
    }

    display("Failed to connect").clear().print();
    delay(2000);
    return false;
  }

  static void maintainConnection() {
    static bool wasConnected = true;
    static unsigned long lastReconnectAttempt = 0;

    if (WiFi.status() == WL_CONNECTED) {
      if (!wasConnected) {
        Serial.println("WiFi reconnected.");
        display("WiFi restored").row(3).print();
        wasConnected = true;
      }
      return;
    }

    if (wasConnected) {
      Serial.println("WiFi connection lost.");
      display("WiFi lost").row(3).print();
      wasConnected = false;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - lastReconnectAttempt >= (unsigned long)config["WIFI_RECONNECT_INTERVAL"]) {
      lastReconnectAttempt = currentMillis;
      Serial.println("Attempting WiFi reconnect...");
      display("Reconnecting...").row(3).print();
      WiFi.reconnect();
    }
  }

  static void setHostname(char* hostname = (char*)"LEEF") {
    WiFi.setHostname(hostname);
  }
};

#endif // WIFI_MANAGER_H