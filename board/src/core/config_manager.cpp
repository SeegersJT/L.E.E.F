#include "core/config_manager.h"
#include "hardware/display_wrapper.h"
#include "core/logger.h"
#include <map>

void ConfigManager::initialConfig()
{
    config_map["LCD_ADDR"] = 0x27;
    config_map["LCD_COLUMNS"] = 16;
    config_map["LCD_ROWS"] = 2;

    config_map["MOISTURE_SENSOR_PIN_MM01"] = 36;

    config_map["RELAY_PIN_R01"] = 23;

    config_map["LCD_SDA_PIN"] = 21;
    config_map["LCD_SCL_PIN"] = 22;

    config_map["MOISTURE_SENSORS_MIN"] = 1200;
    config_map["MOISTURE_SENSORS_MAX"] = 4095;

    config_map["ACTIVATE_RELAY_THRESHOLD"] = 50;
    config_map["MOISTURE_SENSORS_INTERVAL_MINUTES"] = 5;
    config_map["RELAY_ON_DURATION"] = 5000;

    config_map["PULSE_RECHECK_DELAY"] = 30000;
    config_map["MAX_WATERING_PULSES"] = 5;

    config_map["WIFI_CONNECT_MAX_RETRIES"] = 40;
    config_map["WIFI_CONNECT_RETRY_DELAY"] = 500;
    config_map["WIFI_RECONNECT_INTERVAL"] = 5000;

    config_map["FIREBASE_PUSH_INTERVAL"] = 60000;

    config_map["OTA_CHECK_INTERVAL"] = 3600000;

    config_map["PAIRING_CHECK_INTERVAL"] = 30000;
    config_map["OWNERSHIP_RECHECK_INTERVAL"] = 1800000;
}

bool ConfigManager::readIntsFromINI(const String &path)
{
    if (!SPIFFS.begin(true))
    {
        Logger::log(LogCategory::LOG_CONFIG, "SPIFFS mount failed");
        return false;
    }

    File configFile = SPIFFS.open(path, "r");

    if (!configFile || !configFile.available())
    {
        Logger::log(LogCategory::LOG_CONFIG, "Couldn't open " + path + " - keeping defaults");
        if (configFile)
            configFile.close();
        SPIFFS.end();
        return false;
    }

    String line;
    int keysLoaded = 0;

    while (configFile.available())
    {
        line = configFile.readStringUntil('\n');
        line.trim();

        if (line.length() == 0 || line.startsWith(";"))
        {
            continue;
        }

        int delimiterIndex = line.indexOf('=');

        if (delimiterIndex > 0)
        {
            String key = line.substring(0, delimiterIndex);
            String value = line.substring(delimiterIndex + 1);

            config_map[key] = (int)strtol(value.c_str(), nullptr, 0);
            keysLoaded++;
        }
    }

    configFile.close();
    SPIFFS.end();

    Logger::log(LogCategory::LOG_CONFIG, "Loaded " + String(keysLoaded) + " keys from " + path);
    return true;
}

int &ConfigManager::operator[](const String &key)
{
    return config_map[key];
}

String &ConfigManager::StringAccessor::operator[](const String &key)
{
    return string_map[key];
}

void ConfigManager::readStringsFromINI(const String &path)
{
    if (!SPIFFS.begin(true))
    {
        Logger::log(LogCategory::LOG_CONFIG, "SPIFFS mount failed");
        return;
    }

    File configFile = SPIFFS.open(path, "r");

    if (!configFile || !configFile.available())
    {
        Logger::log(LogCategory::LOG_CONFIG, "Couldn't open " + path + " - leaving values blank");
        if (configFile)
            configFile.close();
        SPIFFS.end();
        return;
    }

    String line;
    while (configFile.available())
    {
        line = configFile.readStringUntil('\n');
        line.trim();

        if (line.length() == 0 || line.startsWith(";"))
        {
            continue;
        }

        int delimiterIndex = line.indexOf('=');

        if (delimiterIndex > 0)
        {
            String key = line.substring(0, delimiterIndex);
            String value = line.substring(delimiterIndex + 1);
            str[key] = value;
        }
    }

    configFile.close();
    SPIFFS.end();

    Logger::log(LogCategory::LOG_CONFIG, "Loaded string config from " + path);
}

void ConfigManager::writeString(const String &path, const String &key, const String &value)
{
    if (!SPIFFS.begin(true))
    {
        Logger::log(LogCategory::LOG_CONFIG, "SPIFFS mount failed");
        return;
    }

    std::map<String, String> entries;
    File readFile = SPIFFS.open(path, "r");
    if (readFile)
    {
        while (readFile.available())
        {
            String line = readFile.readStringUntil('\n');
            line.trim();
            if (line.length() == 0 || line.startsWith(";"))
                continue;
            int delimiterIndex = line.indexOf('=');
            if (delimiterIndex > 0)
            {
                entries[line.substring(0, delimiterIndex)] = line.substring(delimiterIndex + 1);
            }
        }
        readFile.close();
    }

    entries[key] = value;

    File writeFile = SPIFFS.open(path, "w");
    if (!writeFile)
    {
        Logger::log(LogCategory::LOG_CONFIG, "Couldn't open " + path + " for writing");
        SPIFFS.end();
        return;
    }

    for (std::map<String, String>::iterator it = entries.begin(); it != entries.end(); ++it)
    {
        writeFile.println(it->first + "=" + it->second);
    }

    writeFile.close();
    SPIFFS.end();

    str[key] = value;
}