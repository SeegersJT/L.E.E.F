#include "cloud/firebase_service.h"
#include "core/logger.h"

WiFiClientSecure FirebaseService::sslClient;
WiFiClientSecure FirebaseService::sslStreamClient;
AsyncClientClass FirebaseService::asyncClient(FirebaseService::sslClient);
AsyncClientClass FirebaseService::asyncStreamClient(FirebaseService::sslStreamClient);

UserAuth *FirebaseService::userAuth = nullptr;
FirebaseApp FirebaseService::app;
RealtimeDatabase FirebaseService::db;

String FirebaseService::deviceIdValue = "";
String FirebaseService::databaseUrlValue = "";
String FirebaseService::storageBucketValue = "";

void FirebaseService::begin(const String &apiKey, const String &databaseUrlIn, const String &storageBucketIn,
                            const String &deviceEmail, const String &devicePassword)
{
    deviceIdValue = WiFi.macAddress();
    deviceIdValue.replace(":", "");

    databaseUrlValue = databaseUrlIn;
    storageBucketValue = storageBucketIn;

    sslClient.setInsecure();
    sslClient.setHandshakeTimeout(30);

    sslStreamClient.setInsecure();
    sslStreamClient.setHandshakeTimeout(30);

    userAuth = new UserAuth(apiKey, deviceEmail, devicePassword, 3000);

    initializeApp(asyncClient, app, getAuth(*userAuth), authResultCallback, "authTask");

    app.getApp<RealtimeDatabase>(db);
    db.url(databaseUrlValue);
}

void FirebaseService::loop()
{
    app.loop();
}

bool FirebaseService::ready()
{
    return app.ready();
}

const String &FirebaseService::deviceId()
{
    return deviceIdValue;
}

const String &FirebaseService::databaseUrl()
{
    return databaseUrlValue;
}

const String &FirebaseService::storageBucketName()
{
    return storageBucketValue;
}

bool FirebaseService::get(const String &path, String &responseOut)
{
    responseOut = db.get<String>(asyncClient, path);
    bool ok = asyncClient.lastError().code() == 0;

    if (!ok)
    {
        Logger::log(LogCategory::LOG_FIREBASE, "GET " + path + " failed: " + asyncClient.lastError().message());
    }

    return ok;
}

bool FirebaseService::put(const String &path, const String &jsonObjectPayload)
{
    bool ok = db.set<object_t>(asyncClient, path, object_t(jsonObjectPayload));

    if (!ok)
    {
        Logger::log(LogCategory::LOG_FIREBASE, "PUT " + path + " failed: " + asyncClient.lastError().message());
    }

    return ok;
}

bool FirebaseService::putString(const String &path, const String &value)
{
    bool ok = db.set<String>(asyncClient, path, value);

    if (!ok)
    {
        Logger::log(LogCategory::LOG_FIREBASE, "PUT " + path + " failed: " + asyncClient.lastError().message());
    }

    return ok;
}

bool FirebaseService::del(const String &path)
{
    bool ok = db.remove(asyncClient, path);

    if (!ok)
    {
        Logger::log(LogCategory::LOG_FIREBASE, "DELETE " + path + " failed: " + asyncClient.lastError().message());
    }

    return ok;
}

RealtimeDatabase &FirebaseService::database()
{
    return db;
}

AsyncClientClass &FirebaseService::client()
{
    return asyncClient;
}

AsyncClientClass &FirebaseService::streamClient()
{
    return asyncStreamClient;
}

void FirebaseService::authResultCallback(AsyncResult &aResult)
{
    if (!aResult.isResult())
        return;

    if (aResult.isError())
    {
        Logger::log(LogCategory::LOG_FIREBASE, "Auth error: " + aResult.error().message());
    }
}