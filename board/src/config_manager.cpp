#include "config_manager.h"
#include "display_wrapper.h"
#include <map>

void ConfigManager::initialConfig() {
  config_map["LCD_ADDR"] = 0x27;
  config_map["LCD_COLUMNS"] = 20;
  config_map["LCD_ROWS"] = 4;

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
}

void ConfigManager::readFromINI() {

  display("Read Config").clear().print();
  display("Initiating...").bottom().print();
  delay(2000);

  if(!SPIFFS.begin(true))
  {
    Serial.println("An error has occurred while mounting SPIFFS");
    display("Read Config").clear().print();
    display("Error Occurred").bottom().print();
    delay(500);
    return;
  }

  File configFile = SPIFFS.open("/config/hardware.ini", "r");

  if (!configFile.available())
  {
    Serial.println("Failed to open config file. Cloning example files.");
    display("Couldn't read config").clear().print();
    display("Sticking to defaults").bottom().print();
    configFile.close();
    delay(500);
  }

  // Check file size and initial contents
  size_t fileSize = configFile.size();
  Serial.println("Config file size: " + String(fileSize));
  if (fileSize == 0)
  {
    Serial.println("Config file is empty.");
    display("Read Config").clear().print();
    display("Config Empty").bottom().print();
    delay(5000);
    configFile.close();
    SPIFFS.end();
    return;
  }

  String fileContents = configFile.readString();
  Serial.println("Config file contents:\n" + fileContents);
  configFile.seek(0);

  Serial.println("Reading config file...");
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

      config_map[key] = value.toInt();
    }
  }

  display("Config read").clear().print();
  display("successfuly").bottom().print();
  delay(2000);

  configFile.close();
  SPIFFS.end();
}

int& ConfigManager::operator[](const String& key) {
  return config_map[key];
}