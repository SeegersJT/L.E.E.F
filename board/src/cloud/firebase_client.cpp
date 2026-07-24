#include "cloud/firebase_client.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "core/logger.h"

String FirebaseClient::apiKey = "";
String FirebaseClient::databaseUrlValue = "";
String FirebaseClient::storageBucketValue = "";
String FirebaseClient::deviceEmail = "";
String FirebaseClient::devicePassword = "";
String FirebaseClient::deviceIdValue = "";

String FirebaseClient::idToken = "";
String FirebaseClient::refreshToken = "";
unsigned long FirebaseClient::tokenExpiresAt = 0;

void FirebaseClient::begin(const String &apiKeyIn, const String &databaseUrlIn, const String &storageBucketIn,
                           const String &deviceEmailIn, const String &devicePasswordIn)
{
    apiKey = apiKeyIn;
    databaseUrlValue = databaseUrlIn;
    storageBucketValue = storageBucketIn;
    deviceEmail = deviceEmailIn;
    devicePassword = devicePasswordIn;

    deviceIdValue = WiFi.macAddress();
    deviceIdValue.replace(":", "");
}

const String &FirebaseClient::deviceId()
{
    return deviceIdValue;
}

const String &FirebaseClient::databaseUrl()
{
    return databaseUrlValue;
}

const String &FirebaseClient::storageBucketName()
{
    return storageBucketValue;
}

String FirebaseClient::authorizationHeaderValue()
{
    return "Bearer " + idToken;
}

bool FirebaseClient::get(const String &path, String &responseOut, const String &extraQuery)
{
    if (!ensureAuthenticated())
        return false;

    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    http.begin(client, databaseUrlValue + path + ".json?auth=" + idToken + extraQuery);
    int responseCode = http.GET();

    if (responseCode != 200)
    {
        Logger::log(LogCategory::LOG_FIREBASE, "GET " + path + " failed: " + String(responseCode));
        http.end();
        return false;
    }

    responseOut = http.getString();
    http.end();
    return true;
}

bool FirebaseClient::put(const String &path, const String &jsonPayload)
{
    if (!ensureAuthenticated())
        return false;

    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    http.begin(client, databaseUrlValue + path + ".json?auth=" + idToken);
    http.addHeader("Content-Type", "application/json");
    int responseCode = http.PUT(jsonPayload);
    http.end();

    if (responseCode <= 0)
    {
        Logger::log(LogCategory::LOG_FIREBASE, "PUT " + path + " failed: " + String(responseCode));
        return false;
    }

    return true;
}

bool FirebaseClient::del(const String &path)
{
    if (!ensureAuthenticated())
        return false;

    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

    HTTPClient http;
    http.begin(client, databaseUrlValue + path + ".json?auth=" + idToken);
    int responseCode = http.sendRequest("DELETE");
    http.end();

    if (responseCode <= 0)
    {
        Logger::log(LogCategory::LOG_FIREBASE, "DELETE " + path + " failed: " + String(responseCode));
        return false;
    }

    return true;
}

bool FirebaseClient::extractJsonValue(const String &json, const char *fieldName, String &out)
{
    String searchKey = String("\"") + fieldName + "\"";
    int start = json.indexOf(searchKey);
    if (start == -1)
        return false;
    start += searchKey.length();

    while (start < (int)json.length() && (json[start] == ' ' || json[start] == ':'))
        start++;

    if (start >= (int)json.length() || json[start] != '"')
        return false;
    start++;

    int end = json.indexOf('"', start);
    if (end == -1)
        return false;
    out = json.substring(start, end);
    return true;
}

String FirebaseClient::urlEncodePathSegment(const String &value)
{
    String encoded = value;
    encoded.replace(" ", "%20");
    encoded.replace(":", "%3A");
    return encoded;
}

bool FirebaseClient::ensureAuthenticated()
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

bool FirebaseClient::signIn()
{
    for (int attempt = 0; attempt < 3; attempt++)
    {
        WiFiClientSecure client;
        client.setInsecure();
        client.setHandshakeTimeout(30);

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

        if (responseCode == 200)
        {
            String response = http.getString();

            bool success = extractJsonValue(response, "idToken", idToken) && extractJsonValue(response, "refreshToken", refreshToken);

            http.end();

            if (success)
            {
                tokenExpiresAt = millis() + 3000UL * 1000UL;
                Logger::log(LogCategory::LOG_FIREBASE, "Signed in");
                return true;
            }
        }
        else
        {
            Logger::log(LogCategory::LOG_FIREBASE, "Sign-in attempt " + String(attempt + 1) + " failed: " + String(responseCode));
        }

        http.end();
        delay(1000);
    }

    return false;
}

bool FirebaseClient::refreshIdToken()
{
    WiFiClientSecure client;
    client.setInsecure();
    client.setHandshakeTimeout(30);

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
        success = extractJsonValue(response, "\"id_token\":\"", idToken) && extractJsonValue(response, "\"refresh_token\":\"", refreshToken);
        if (success)
        {
            tokenExpiresAt = millis() + 3000UL * 1000UL;
            Logger::log(LogCategory::LOG_FIREBASE, "Token refreshed");
        }
    }

    http.end();
    return success;
}