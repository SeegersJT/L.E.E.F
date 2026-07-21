#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "globals.h"

class WiFiManager
{
public:
  static bool connectToWiFi(int maxRetries = config["WIFI_CONNECT_MAX_RETRIES"], unsigned long retryDelay = config["WIFI_CONNECT_RETRY_DELAY"])
  {
    display("Connecting WiFi").clear().print();

    String ssid = config.str["WIFI_SSID"];
    String password = config.str["WIFI_PASSWORD"];

    if (ssid.length() == 0)
    {
      Serial.println("No WiFi SSID configured - skipping STA connect.");
      WiFi.setAutoReconnect(false);
      WiFi.disconnect(true, true);
      display("No WiFi configured").clear().print();
      delay(1000);
      return false;
    }

    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid.c_str(), password.c_str());

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < maxRetries)
    {
      delay(retryDelay);
      retries++;
      display("Connecting... " + String(retries) + "/" + String(maxRetries)).bottom().print();
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      display("Connected").clear().print();
      display(WiFi.localIP().toString()).bottom().print();
      delay(2000);
      return true;
    }

    display("Failed to connect").clear().print();
    delay(2000);
    return false;
  }

  static void maintainConnection()
  {
    static bool wasConnected = false;
    static unsigned long lastReconnectAttempt = 0;

    if (WiFi.status() == WL_CONNECTED)
    {
      if (!wasConnected)
      {
        Serial.println("WiFi reconnected.");
        display("WiFi restored").row(3).print();
        wasConnected = true;
      }
      return;
    }
    
    if (config.str["WIFI_SSID"].length() == 0)
    {
      return;
    }

    if (wasConnected)
    {
      Serial.println("WiFi connection lost.");
      display("WiFi lost").row(3).print();
      wasConnected = false;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - lastReconnectAttempt >= (unsigned long)config["WIFI_RECONNECT_INTERVAL"])
    {
      lastReconnectAttempt = currentMillis;
      Serial.println("Attempting WiFi reconnect...");
      display("Reconnecting...").row(3).print();
      WiFi.reconnect();
    }
  }
  static void setHostname(char *hostname = (char *)"LEEF")
  {
    WiFi.setHostname(hostname);
  }
};

#endif // WIFI_MANAGER_H