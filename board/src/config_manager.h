#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <FS.h>
#include <SPIFFS.h>
#include <map>
#include <Arduino.h>
#include "display_wrapper.h"

class ConfigManager
{
public:
    void initialConfig();
    void readFromINI();
    void readStringsFromINI(const String &path);
    void writeString(const String &path, const String &key, const String &value);

    std::map<String, int> config_map;
    int &operator[](const String &key);
    class StringAccessor
    {
    public:
        std::map<String, String> string_map;
        String &operator[](const String &key);
    };
    StringAccessor str;
};

#endif // CONFIG_MANAGER_H