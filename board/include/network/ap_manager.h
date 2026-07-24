#ifndef AP_MANAGER_H
#define AP_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <vector>
#include <algorithm>
#include "core/globals.h"
#include "network/portal_pages.h"
#include "core/logger.h"

class APManager
{
public:
    static WebServer webServer;
    static DNSServer dnsServer;

    static void enableAccessPoint(const char *ssid = "L.E.E.F. SETUP")
    {
        apSsid = String(ssid);

        WiFi.mode(WIFI_AP_STA);

        IPAddress local_ip(10, 0, 0, 1);
        IPAddress gateway(10, 0, 0, 1);
        IPAddress subnet(255, 255, 255, 0);

        WiFi.softAPConfig(local_ip, gateway, subnet);
        WiFi.softAP(ssid, NULL);

        IPAddress apIP = WiFi.softAPIP();
        Logger::log(LogCategory::LOG_AP, "Access point IP: " + apIP.toString());

        dnsServer.start(53, "*", apIP);

        webServer.on("/", HTTP_GET, handleRoot);
        webServer.on("/connect", HTTP_POST, handleConnect);
        webServer.onNotFound(handleRoot);
        webServer.begin();

        apActive = true;
    }

    static void handlePortal()
    {
        if (shutdownPending)
        {
            disable();
            shutdownPending = false;
            return;
        }

        if (!apActive)
            return;

        static unsigned long lastToggle = 0;
        static bool showingJoinStep = true;
        unsigned long currentMillis = millis();

        if (currentMillis - lastToggle >= 3000)
        {
            lastToggle = currentMillis;
            showingJoinStep = !showingJoinStep;

            if (showingJoinStep)
            {
                display("Join WiFi:").clear().print();
                display(apSsid).row(1).print();
            }
            else
            {
                display("Then visit:").clear().print();
                display("10.0.0.1").row(1).print();
            }
        }

        dnsServer.processNextRequest();
        webServer.handleClient();
    }

private:
    static String apSsid;
    static bool apActive;
    static bool shutdownPending;

    static void disable()
    {
        dnsServer.stop();
        webServer.stop();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        apActive = false;
        Logger::log(LogCategory::LOG_AP, "Access point disabled - WiFi connected");
    }

    static void handleRoot()
    {
        Logger::log(LogCategory::LOG_AP, "Portal page served: " + webServer.uri());

        int networkCount = WiFi.scanNetworks();

        std::vector<std::pair<String, int>> networks;
        for (int i = 0; i < networkCount; i++)
        {
            String ssid = WiFi.SSID(i);
            if (ssid.length() == 0)
                continue;

            int rssi = WiFi.RSSI(i);
            bool found = false;
            for (auto &n : networks)
            {
                if (n.first == ssid)
                {
                    found = true;
                    if (rssi > n.second)
                        n.second = rssi;
                    break;
                }
            }
            if (!found)
                networks.push_back(std::make_pair(ssid, rssi));
        }

        std::sort(networks.begin(), networks.end(), [](const std::pair<String, int> &a, const std::pair<String, int> &b)
                  { return a.second > b.second; });

        webServer.send(200, "text/html", buildNetworkPickerPage(networks));
    }

    static void handleConnect()
    {
        String ssid = webServer.arg("ssid");
        String password = webServer.arg("password");

        if (ssid.length() == 0)
        {
            Logger::log(LogCategory::LOG_AP, "Rejected /connect with empty SSID");
            webServer.send(200, "text/html", buildConnectFailedPage());
            return;
        }

        display("Connecting to").clear().print();
        display(ssid).bottom().print();

        WiFi.begin(ssid.c_str(), password.c_str());

        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
        {
            delay(250);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            config.writeString("/config/wifi_saved.ini", "WIFI_SAVED_SSID", ssid);
            config.writeString("/config/wifi_saved.ini", "WIFI_SAVED_PASSWORD", password);
            Logger::log(LogCategory::LOG_AP, "WiFi credentials saved for future boots");

            display("Connected!").clear().print();
            display(WiFi.localIP().toString()).bottom().print();

            webServer.send(200, "text/html", buildConnectedPage(WiFi.localIP().toString()));
            shutdownPending = true;
        }
        else
        {
            display("Connect failed").clear().print();
            display("Try again").bottom().print();

            webServer.send(200, "text/html", buildConnectFailedPage());
        }
    }
};

#endif // AP_MANAGER_H