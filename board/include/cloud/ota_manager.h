#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>

class OtaManager
{
public:
    static void begin(const String &apiKey, const String &deviceEmail, const String &devicePassword);

    static void checkForUpdate();

    static const String &lastResult();
    static const String &lastCheckTime();

private:
    static String apiKey;
    static String deviceEmail;
    static String devicePassword;
    static String idToken;
    static String refreshToken;
    static unsigned long tokenExpiresAt;

    static unsigned long lastOtaCheck;
    static String lastOtaResult;
    static String lastOtaCheckTime;

    static void mirrorFirmwareEntry(const String &key, const String &date, const String &filename, const String &url);
    static bool applyFirmware(const String &filename);

    static bool ensureAuthenticated();
    static bool signIn();
    static bool refreshIdToken();
    static bool extractJsonValue(const String &json, const char *fieldName, String &out);
    static int findMatchingBrace(const String &text, int openIndex);
};

#endif // OTA_MANAGER_H