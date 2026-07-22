#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "globals.h"
#include "logger.h"

class WiFiManager
{
public:
  static bool connectToWiFi(const String &ssid, const String &password,
                            int maxRetries = config["WIFI_CONNECT_MAX_RETRIES"], unsigned long retryDelay = config["WIFI_CONNECT_RETRY_DELAY"])
  {
    display("Connecting WiFi").clear().print();

    if (ssid.length() == 0)
    {
      Logger::log(LogCategory::LOG_WIFI, "No SSID configured, skipping STA connect");

      WiFi.setAutoReconnect(false);
      WiFi.disconnect(true, true);

      display("No WiFi found").clear().print();

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

      display("Try " + String(retries) + "/" + String(maxRetries)).bottom().print();
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      display("Connected").clear().print();
      display(WiFi.localIP().toString()).bottom().print();

      delay(2000);

      return true;
    }

    display("Connect Failed").clear().print();

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
        Logger::log(LogCategory::LOG_WIFI, "Reconnected");

        display("WiFi Status").clear().top().print();
        display("Restored").bottom().print();

        wasConnected = true;
      }

      return;
    }

    if (config.str["WIFI_SAVED_SSID"].length() == 0 && config.str["WIFI_SSID"].length() == 0)
    {
      return;
    }

    if (wasConnected)
    {
      Logger::log(LogCategory::LOG_WIFI, "Connection lost");

      display("WiFi Status").clear().top().print();
      display("Lost connection").bottom().print();

      wasConnected = false;
    }

    unsigned long currentMillis = millis();

    if (currentMillis - lastReconnectAttempt >= (unsigned long)config["WIFI_RECONNECT_INTERVAL"])
    {
      lastReconnectAttempt = currentMillis;
      Logger::log(LogCategory::LOG_WIFI, "Attempting reconnect");
      display("WiFi Status").clear().top().print();
      display("Reconnecting...").bottom().print();
      WiFi.reconnect();
    }
  }
  static void setHostname(char *hostname = (char *)"LEEF")
  {
    WiFi.setHostname(hostname);
  }
};

#endif // WIFI_MANAGER_H