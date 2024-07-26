#include "config_manager.h"

int LCD_ADDR = 0x27;
int LCD_COLUMNS = 20;
int LCD_ROWS = 4;
int MOISTURE_SENSOR_PIN_MM01 = 36;
int RELAY_PIN_R01 = 23;
int LCD_SDA_PIN = 21;
int LCD_SCL_PIN = 22;
int MOISTURE_SENSORS_MIN = 1200;
int MOISTURE_SENSORS_MAX = 4095;
int ACTIVATE_RELAY_THRESHOLD = 50;
int MOISTURE_SENSORS_INTERVAL_MINUTES = 5;
unsigned long MOISTURE_SENSORS_CHECK_INTERVAL = MOISTURE_SENSORS_INTERVAL_MINUTES * 60 * 1000;
unsigned long RELAY_ON_DURATION = 1000;

void readConfig() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }

    File configFile = SPIFFS.open("/config.ini", "r");

    if (!configFile) {
        Serial.println("Failed to open config file. Using default values.");
        return;
    }

    String line;
    while (configFile.available()) {
        line = configFile.readStringUntil('\n');
        line.trim();

        if (line.length() == 0 || line.startsWith(";")) {
            continue;
        }

        int delimiterIndex = line.indexOf('=');
        if (delimiterIndex > 0) {
            String key = line.substring(0, delimiterIndex);
            String value = line.substring(delimiterIndex + 1);

            if (key == "LCD_ADDR") {
                LCD_ADDR = value.toInt();
            } else if (key == "LCD_COLUMNS") {
                LCD_COLUMNS = value.toInt();
            } else if (key == "LCD_ROWS") {
                LCD_ROWS = value.toInt();
            } else if (key == "MOISTURE_SENSOR_PIN_MM01") {
                MOISTURE_SENSOR_PIN_MM01 = value.toInt();
            } else if (key == "RELAY_PIN_R01") {
                RELAY_PIN_R01 = value.toInt();
            } else if (key == "LCD_SDA_PIN") {
                LCD_SDA_PIN = value.toInt();
            } else if (key == "LCD_SCL_PIN") {
                LCD_SCL_PIN = value.toInt();
            } else if (key == "MOISTURE_SENSORS_MIN") {
                MOISTURE_SENSORS_MIN = value.toInt();
            } else if (key == "MOISTURE_SENSORS_MAX") {
                MOISTURE_SENSORS_MAX = value.toInt();
            } else if (key == "ACTIVATE_RELAY_THRESHOLD") {
                ACTIVATE_RELAY_THRESHOLD = value.toInt();
            } else if (key == "MOISTURE_SENSORS_INTERVAL_MINUTES") {
                MOISTURE_SENSORS_INTERVAL_MINUTES = value.toInt();
                MOISTURE_SENSORS_CHECK_INTERVAL = MOISTURE_SENSORS_INTERVAL_MINUTES * 60 * 1000;
            } else if (key == "RELAY_ON_DURATION") {
                RELAY_ON_DURATION = value.toInt();
            }
        }
    }

    configFile.close();
}
