#ifndef FIREBASE_SERVICE_H
#define FIREBASE_SERVICE_H

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

class FirebaseService
{
public:
    static void begin(const String &apiKey, const String &databaseUrl, const String &storageBucket, const String &deviceEmail, const String &devicePassword);

    static void loop();

    static bool ready();

    static const String &deviceId();
    static const String &databaseUrl();
    static const String &storageBucketName();

    static bool get(const String &path, String &responseOut);
    static bool put(const String &path, const String &jsonObjectPayload);
    static bool putString(const String &path, const String &value);
    static bool del(const String &path);

    static RealtimeDatabase &database();
    static AsyncClientClass &client();
    static AsyncClientClass &streamClient();

private:
    static WiFiClientSecure sslClient;
    static WiFiClientSecure sslStreamClient;
    static AsyncClientClass asyncClient;
    static AsyncClientClass asyncStreamClient;

    static UserAuth *userAuth;
    static FirebaseApp app;
    static RealtimeDatabase db;

    static String deviceIdValue;
    static String databaseUrlValue;
    static String storageBucketValue;

    static void authResultCallback(AsyncResult &aResult);
};

#endif // FIREBASE_SERVICE_H