#ifndef AP_MANAGER_H
#define AP_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "globals.h"

class APManager
{
public:
    static WebServer webServer;
    static DNSServer dnsServer;

    static void enableAccessPoint(const char *ssid = "L.E.E.F.")
    {
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

        display("AP Ready").clear().print();
        display(String(ssid)).bottom().print();
    }

    static void handlePortal()
    {
        if (!apActive)
            return;
        dnsServer.processNextRequest();
        webServer.handleClient();
    }

private:
    static bool apActive;

    static void handleRoot()
    {
        int networkCount = WiFi.scanNetworks();

        String page = "<!DOCTYPE html><html><head><title>L.E.E.F. Setup</title>";
        page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
        page += "<style>body{font-family:sans-serif;padding:16px;}label{display:block;margin-top:12px;}";
        page += "input,select{width:100%;padding:8px;box-sizing:border-box;}";
        page += "button{margin-top:16px;padding:10px;width:100%;}</style>";
        page += "</head><body>";
        page += "<h2>Connect L.E.E.F. to WiFi</h2>";
        page += "<form action='/connect' method='POST'>";
        page += "<label for='ssid'>Network</label>";
        page += "<select name='ssid' id='ssid'>";

        for (int i = 0; i < networkCount; i++)
        {
            String netSsid = WiFi.SSID(i);
            if (netSsid.length() == 0)
                continue;

            page += "<option value='" + netSsid + "'>" + netSsid + " (" + String(WiFi.RSSI(i)) + " dBm)</option>";
        }

        page += "</select>";
        page += "<label for='password'>Password</label>";
        page += "<input type='password' name='password' id='password'>";
        page += "<button type='submit'>Connect</button>";
        page += "</form>";
        page += "</body></html>";

        webServer.send(200, "text/html", page);
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

            webServer.send(200, "text/html",
                           "<!DOCTYPE html><html><body><h2>Connected!</h2>"
                           "<p>L.E.E.F. is now on your WiFi network.</p></body></html>");
        }
        else
        {
            display("Connect failed").clear().print();
            display("Try again").bottom().print();

            webServer.send(200, "text/html",
                           "<!DOCTYPE html><html><body><h2>Couldn't connect</h2>"
                           "<p>Check the password and <a href='/'>try again</a>.</p></body></html>");
        }
    }
};

#endif // AP_MANAGER_H