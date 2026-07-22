#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum class LogCategory
{
    LOG_BOOT,
    LOG_CONFIG,
    LOG_DISPLAY,
    LOG_WIFI,
    LOG_AP,
    LOG_FIREBASE,
    LOG_OTA,
    LOG_WATERING,
    LOG_DEVICE
};

class Logger
{
public:
    static void log(LogCategory category, const String &message)
    {
        Serial.println(formatLine(category, message));
    }

private:
    static const char *categoryLabel(LogCategory category)
    {
        switch (category)
        {
        case LogCategory::LOG_BOOT:
            return "BOOT";
        case LogCategory::LOG_CONFIG:
            return "CONFIG";
        case LogCategory::LOG_DISPLAY:
            return "DISPLAY";
        case LogCategory::LOG_WIFI:
            return "WIFI";
        case LogCategory::LOG_AP:
            return "AP";
        case LogCategory::LOG_FIREBASE:
            return "FIREBASE";
        case LogCategory::LOG_OTA:
            return "OTA";
        case LogCategory::LOG_WATERING:
            return "WATERING";
        case LogCategory::LOG_DEVICE:
            return "DEVICE";
        default:
            return "LOG";
        }
    }

    static String formatLine(LogCategory category, const String &message)
    {
        char stamp[16];
        snprintf(stamp, sizeof(stamp), "[%10lu]", millis());
        return String(stamp) + " [" + categoryLabel(category) + "] " + message;
    }
};

#endif // LOGGER_H