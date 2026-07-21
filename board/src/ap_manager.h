#ifndef AP_MANAGER_H
#define AP_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <vector>
#include <algorithm>
#include "globals.h"
#include "portal_pages.h"

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
        Serial.print("AP IP Address: ");
        Serial.println(apIP);

        dnsServer.start(53, "*", apIP);

        webServer.on("/", HTTP_GET, handleRoot);
        webServer.on("/connect", HTTP_POST, handleConnect);
        webServer.onNotFound(handleRoot);
        webServer.begin();

        apActive = true;
    }

    static void handlePortal()
    {
        if (!apActive)
            return;

        static unsigned long lastToggle = 0;
        static bool showingJoinStep = true;
        unsigned long currentMillis = millis();

        if (currentMillis - lastToggle >= 3000)
        {
            lastToggle = currentMillis;
            showingJoinStep = !showingJoinStep;

            display("Then visit:").clear().print();
            display("10.0.0.1").row(1).print();

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

    static void handleRoot()
    {
        Serial.println("Portal page served, path: " + webServer.uri());

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
            display("Connected!").clear().print();
            display(WiFi.localIP().toString()).bottom().print();

            webServer.send(200, "text/html", buildConnectedPage(WiFi.localIP().toString()));
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