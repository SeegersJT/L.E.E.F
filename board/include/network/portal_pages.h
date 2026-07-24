#ifndef PORTAL_PAGES_H
#define PORTAL_PAGES_H

#include <Arduino.h>
#include <vector>
#include <utility>

inline String portalLeafIcon(int size = 28)
{
    String svg = "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='none' stroke='currentColor' stroke-width='2' stroke-linecap='round' stroke-linejoin='round' width='";
    svg += String(size);
    svg += "' height='";
    svg += String(size);
    svg += "'>";
    svg += "<path d='M11 20A7 7 0 0 1 9.8 6.1C15.5 5 17 4.48 19 2c1 2 2 4.18 2 8 0 5.5-4.78 10-11 10Z'/>";
    svg += "<path d='M2 21c0-3 1.85-5.36 5.08-6C9.5 14.52 12 13 13 12'/>";
    svg += "</svg>";
    return svg;
}

inline String portalPageStyle()
{
    String css = "<style>";
    css += ":root{";
    css += "--background:#f4f7f5;--foreground:#1c2b22;--card:#ffffff;";
    css += "--primary:#2f7a4f;--primary-foreground:#ffffff;--primary-dark:#1f5c3a;";
    css += "--muted:#eef2ef;--muted-foreground:#6b7d72;";
    css += "--border:#dfe6e1;--input:#dfe6e1;--destructive:#d64545;";
    css += "--radius-2xl:24px;--radius-3xl:28px;";
    css += "--shadow-soft:0 1px 2px rgba(31,92,58,.04),0 8px 24px -12px rgba(31,92,58,.12);";
    css += "--shadow-card:0 1px 2px rgba(31,92,58,.05),0 12px 32px -16px rgba(31,92,58,.16);";
    css += "}";
    css += "*{box-sizing:border-box;}";
    css += "body{margin:0;min-height:100vh;padding:40px 24px;";
    css += "font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;";
    css += "background:var(--background);color:var(--foreground);}";
    css += ".shell{max-width:384px;margin:0 auto;}";
    css += ".shell-inner{display:flex;flex-direction:column;align-items:center;text-align:center;}";
    css += ".logo-mark{width:64px;height:64px;border-radius:var(--radius-3xl);background:var(--primary);";
    css += "color:var(--primary-foreground);display:grid;place-items:center;box-shadow:var(--shadow-soft);}";
    css += ".logo-mark--alert{background:var(--destructive);}";
    css += "h1{margin:20px 0 0;font-size:28px;font-weight:900;letter-spacing:-0.02em;}";
    css += "p.sub{margin:4px 0 0;font-size:14px;color:var(--muted-foreground);}";
    css += "form{text-align:left;margin-top:24px;}";
    css += "label{display:block;font-size:12px;font-weight:700;color:var(--muted-foreground);";
    css += "margin:16px 0 6px;text-transform:uppercase;letter-spacing:0.05em;}";
    css += "select,input{width:100%;height:48px;padding:0 16px;border:1px solid var(--input);";
    css += "border-radius:var(--radius-2xl);font-size:14px;background:var(--card);color:var(--foreground);}";
    css += "select:focus,input:focus{outline:none;border-color:var(--primary);";
    css += "box-shadow:0 0 0 4px rgba(47,122,79,0.2);}";
    css += "button{width:100%;height:48px;margin-top:24px;border:none;border-radius:9999px;";
    css += "background:var(--primary);color:var(--primary-foreground);font-size:14px;font-weight:700;";
    css += "cursor:pointer;box-shadow:var(--shadow-soft);}";
    css += "button:active{background:var(--primary-dark);}";
    css += "a{color:var(--primary-dark);font-weight:700;text-decoration:none;}";
    css += "</style>";
    return css;
}

inline String portalSignalQuality(int rssi)
{
    if (rssi > -55)
        return "Excellent";
    if (rssi > -67)
        return "Good";
    if (rssi > -75)
        return "Fair";
    return "Weak";
}

inline String buildNetworkPickerPage(const std::vector<std::pair<String, int>> &networks)
{
    String page = "<!DOCTYPE html><html><head><title>L.E.E.F. Setup</title>";
    page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    page += portalPageStyle();
    page += "</head><body><div class='shell'><div class='shell-inner'>";
    page += "<div class='logo-mark'>" + portalLeafIcon() + "</div>";
    page += "<h1>L.E.E.F.</h1>";
    page += "<p class='sub'>Pick your WiFi network to get your plant online</p>";
    page += "</div>";
    page += "<form action='/connect' method='POST'>";
    page += "<label for='ssid'>Network</label>";
    page += "<select name='ssid' id='ssid'>";

    for (const auto &n : networks)
    {
        page += "<option value='" + n.first + "'>" + n.first + " &middot; " + portalSignalQuality(n.second) + "</option>";
    }

    page += "</select>";
    page += "<label for='password'>Password</label>";
    page += "<input type='password' name='password' id='password' placeholder='WiFi password'>";
    page += "<button type='submit'>Connect</button>";
    page += "</form></div></body></html>";
    return page;
}

inline String buildConnectedPage(const String &ipAddress)
{
    String page = "<!DOCTYPE html><html><head><title>Connected</title>";
    page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    page += portalPageStyle();
    page += "</head><body><div class='shell'><div class='shell-inner'>";
    page += "<div class='logo-mark'>" + portalLeafIcon() + "</div>";
    page += "<h1>L.E.E.F.</h1>";
    page += "<p class='sub'>Connected! You can close this page - your plant is coming online.</p>";
    page += "</div></div></body></html>";
    return page;
}

inline String buildConnectFailedPage()
{
    String page = "<!DOCTYPE html><html><head><title>Connection Failed</title>";
    page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    page += portalPageStyle();
    page += "</head><body><div class='shell'><div class='shell-inner'>";
    page += "<div class='logo-mark logo-mark--alert'>" + portalLeafIcon() + "</div>";
    page += "<h1>L.E.E.F.</h1>";
    page += "<p class='sub'>Couldn't connect. Double check the password and <a href='/'>try again</a>.</p>";
    page += "</div></div></body></html>";
    return page;
}

#endif // PORTAL_PAGES_H