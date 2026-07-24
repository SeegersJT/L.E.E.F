#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>

class OtaManager
{
public:
    static void checkForUpdate();

    static const String &lastResult();
    static const String &lastCheckTime();

private:
    static unsigned long lastOtaCheck;
    static String lastOtaResult;
    static String lastOtaCheckTime;

    static void mirrorFirmwareEntry(const String &key, const String &date, const String &filename, const String &url);
    static bool applyFirmware(const String &filename);
};

#endif // OTA_MANAGER_H