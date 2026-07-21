#ifndef PORTAL_PAGES_H
#define PORTAL_PAGES_H

#include <Arduino.h>
#include <vector>
#include <utility>

inline String portalPageStyle() {
  String css = "<style>";
  css += ":root{--green:#2f7a4f;--green-dark:#1f5c3a;--bg:#f4f7f5;--text:#1c2b22;--muted:#6b7d72;}";
  css += "*{box-sizing:border-box;}";
  css += "body{margin:0;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:var(--bg);color:var(--text);display:flex;min-height:100vh;align-items:center;justify-content:center;padding:24px;}";
  css += ".card{background:#fff;border-radius:16px;box-shadow:0 8px 24px rgba(0,0,0,0.08);padding:32px 28px;max-width:380px;width:100%;text-align:center;}";
  css += ".logo{font-size:40px;margin-bottom:4px;}";
  css += "h1{font-size:20px;margin:0 0 4px;color:var(--green-dark);}";
  css += "p.sub{color:var(--muted);font-size:14px;margin:0 0 24px;}";
  css += "form{text-align:left;}";
  css += "label{display:block;font-size:13px;font-weight:600;color:var(--muted);margin:16px 0 6px;text-transform:uppercase;letter-spacing:0.03em;}";
  css += "select,input{width:100%;padding:12px 14px;border:1px solid #dfe6e1;border-radius:10px;font-size:15px;background:#fafcfb;}";
  css += "select:focus,input:focus{outline:none;border-color:var(--green);}";
  css += "button{width:100%;margin-top:24px;padding:14px;border:none;border-radius:10px;background:var(--green);color:#fff;font-size:16px;font-weight:600;cursor:pointer;}";
  css += "button:active{background:var(--green-dark);}";
  css += "a{color:var(--green-dark);font-weight:600;}";
  css += "</style>";
  return css;
}

inline String portalSignalQuality(int rssi) {
  if (rssi > -55) return "Excellent";
  if (rssi > -67) return "Good";
  if (rssi > -75) return "Fair";
  return "Weak";
}

inline String buildNetworkPickerPage(const std::vector<std::pair<String, int>>& networks) {
  String page = "<!DOCTYPE html><html><head><title>L.E.E.F. Setup</title>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += portalPageStyle();
  page += "</head><body><div class='card'>";
  page += "<div class='logo'>&#127807;</div>";
  page += "<h1>Connect L.E.E.F.</h1>";
  page += "<p class='sub'>Pick your WiFi network to get your plant online</p>";
  page += "<form action='/connect' method='POST'>";
  page += "<label for='ssid'>Network</label>";
  page += "<select name='ssid' id='ssid'>";

  for (const auto& n : networks) {
    page += "<option value='" + n.first + "'>" + n.first + " &middot; " + portalSignalQuality(n.second) + "</option>";
  }

  page += "</select>";
  page += "<label for='password'>Password</label>";
  page += "<input type='password' name='password' id='password' placeholder='WiFi password'>";
  page += "<button type='submit'>Connect</button>";
  page += "</form></div></body></html>";
  return page;
}

inline String buildConnectedPage(const String& ipAddress) {
  String page = "<!DOCTYPE html><html><head><title>Connected</title>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += portalPageStyle();
  page += "</head><body><div class='card'>";
  page += "<div class='logo'>&#9989;</div>";
  page += "<h1>Connected!</h1>";
  page += "<p class='sub'>L.E.E.F. is now on your WiFi network. You can close this page.</p>";
  page += "</div></body></html>";
  return page;
}

inline String buildConnectFailedPage() {
  String page = "<!DOCTYPE html><html><head><title>Connection Failed</title>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += portalPageStyle();
  page += "</head><body><div class='card'>";
  page += "<div class='logo'>&#9888;&#65039;</div>";
  page += "<h1>Couldn't Connect</h1>";
  page += "<p class='sub'>Double check the password and <a href='/'>try again</a>.</p>";
  page += "</div></body></html>";
  return page;
}

#endif // PORTAL_PAGES_H