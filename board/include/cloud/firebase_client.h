#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H

#include <Arduino.h>

class FirebaseClient
{
public:
    static void begin(const String &apiKey, const String &databaseUrl, const String &storageBucket, const String &deviceEmail, const String &devicePassword);

    static const String &deviceId();
    static const String &databaseUrl();
    static const String &storageBucketName();

    static String authorizationHeaderValue();

    static bool get(const String &path, String &responseOut, const String &extraQuery = "");
    static bool put(const String &path, const String &jsonPayload);
    static bool del(const String &path);

    static bool extractJsonValue(const String &json, const char *fieldName, String &out);
    static String urlEncodePathSegment(const String &value);

private:
    static String apiKey;
    static String databaseUrlValue;
    static String storageBucketValue;
    static String deviceEmail;
    static String devicePassword;
    static String deviceIdValue;

    static String idToken;
    static String refreshToken;
    static unsigned long tokenExpiresAt;

    static bool ensureAuthenticated();
    static bool signIn();
    static bool refreshIdToken();
};

#endif // FIREBASE_CLIENT_H