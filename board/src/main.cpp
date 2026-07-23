#include <Arduino.h>
#include "wifi_manager.h"
#include "ap_manager.h"
#include "firebase_manager.h"
#include "time_utils.h"
#include "globals.h"
#include "device_wrapper.h"
#include "logger.h"

DeviceWrapper<MoistureDevice> *moistureSensor_MM01;
DeviceWrapper<RelayDevice> *relay_R01;

WateringState wateringState = WateringState::IDLE;
int wateringPulseCount = 0;

int lastMoisturePercentage = -1;
String lastMoistureTimestamp = "";
String lastRelayTimestamp = "";

void listSPIFFSFiles()
{
  Logger::log(LogCategory::LOG_BOOT, "Listing SPIFFS files");
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file)
  {
    Logger::log(LogCategory::LOG_BOOT, String(file.name()) + " (" + String(file.size()) + " bytes)");
    file = root.openNextFile();
  }
}

void startDisplay()
{
  display::init();
  display("Starting...").print();
  delay(5000);
}

void setupDevices()
{
  display("Device Setup").clear().print();
  display("Initiating...").bottom().print();
  delay(2000);

  moistureSensor_MM01 = new DeviceWrapper<MoistureDevice>(config["MOISTURE_SENSOR_PIN_MM01"], "MOISTURE SENSOR 01");
  relay_R01 = new DeviceWrapper<RelayDevice>(config["RELAY_PIN_R01"], "RELAY 01");

  moistureSensor_MM01->setup();
  relay_R01->setup();

  display("Device Setup").clear().print();
  display("Successful").bottom().print();
  delay(2000);
}

void connectRelayToMoistureSensor(DeviceWrapper<RelayDevice> &relayDevice, DeviceWrapper<MoistureDevice> &moistureDevice)
{
  unsigned long currentMillis = millis();

  switch (wateringState)
  {

  case WateringState::IDLE:
  {
    if (currentMillis - moistureDevice.getTimestamp() >= config["MOISTURE_SENSORS_INTERVAL_MINUTES"] * 60 * 1000)
    {
      int moisturePercentage = moistureDevice.getMoisturePercentage();
      lastMoisturePercentage = moisturePercentage;
      lastMoistureTimestamp = currentIsoTimestamp();
      FirebaseManager::logMoistureReading(moisturePercentage, lastMoistureTimestamp);
      display("Moisture: " + String(moisturePercentage) + "%").clear().print();

      if (moisturePercentage < config["ACTIVATE_RELAY_THRESHOLD"])
      {
        wateringPulseCount = 0;
        relayDevice.setState("ON");
        lastRelayTimestamp = currentIsoTimestamp();
        FirebaseManager::logRelayEvent("ON", lastRelayTimestamp);
        display("Watering... (1)").bottom().print();
        wateringState = WateringState::PULSE_ON;
      }
      else
      {
        display("Moisture OK").bottom().print();
      }
    }
    break;
  }

  case WateringState::PULSE_ON:
  {
    if (currentMillis - relayDevice.getTimestamp() >= config["RELAY_ON_DURATION"])
    {
      relayDevice.setState("OFF");
      lastRelayTimestamp = currentIsoTimestamp();
      FirebaseManager::logRelayEvent("OFF", lastRelayTimestamp);
      wateringPulseCount++;
      wateringState = WateringState::PULSE_SETTLE;
    }
    break;
  }

  case WateringState::PULSE_SETTLE:
  {
    if (currentMillis - relayDevice.getTimestamp() >= config["PULSE_RECHECK_DELAY"])
    {
      int moisturePercentage = moistureDevice.getMoisturePercentage();
      lastMoisturePercentage = moisturePercentage;
      lastMoistureTimestamp = currentIsoTimestamp();
      FirebaseManager::logMoistureReading(moisturePercentage, lastMoistureTimestamp);
      display("Moisture: " + String(moisturePercentage) + "%").clear().print();

      if (moisturePercentage >= config["ACTIVATE_RELAY_THRESHOLD"])
      {
        display("Watering done").bottom().print();
        wateringState = WateringState::IDLE;
      }
      else if (wateringPulseCount >= config["MAX_WATERING_PULSES"])
      {
        display("Pulse limit hit").bottom().print();
        wateringState = WateringState::IDLE;
      }
      else
      {
        relayDevice.setState("ON");
        lastRelayTimestamp = currentIsoTimestamp();
        FirebaseManager::logRelayEvent("ON", lastRelayTimestamp);
        display("Watering... (" + String(wateringPulseCount + 1) + ")").bottom().print();
        wateringState = WateringState::PULSE_ON;
      }
    }
    break;
  }
  }
}

void setup()
{
  Serial.begin(9600);

  config.initialConfig();

  startDisplay();

  if (!SPIFFS.begin(true))
  {
    Logger::log(LogCategory::LOG_BOOT, "SPIFFS mount failed");
    return;
  }

  listSPIFFSFiles();

  display("Loading Config").clear().print();

  bool hardwareOk = config.readIntsFromINI("/config/hardware.ini");
  bool pinsOk = config.readIntsFromINI("/config/pins.ini");
  bool timingsOk = config.readIntsFromINI("/config/timings.ini");
  bool networkOk = config.readIntsFromINI("/config/network.ini");
  bool configOk = hardwareOk && pinsOk && timingsOk && networkOk;

  display(configOk ? "Config Loaded" : "Config Issues").bottom().print();
  delay(1000);

  config.readStringsFromINI("/config/wifi.ini");
  config.readStringsFromINI("/config/wifi_saved.ini");
  config.readStringsFromINI("/config/firebase.ini");
  config.readStringsFromINI("/config/ota_state.ini");

  setupDevices();

  WiFiManager::setHostname();

  bool wifiConnected = WiFiManager::connectToWiFi(config.str["WIFI_SAVED_SSID"], config.str["WIFI_SAVED_PASSWORD"]);

  if (!wifiConnected)
  {
    Logger::log(LogCategory::LOG_WIFI, "Saved WiFi unavailable, trying wifi.ini default");
    wifiConnected = WiFiManager::connectToWiFi(config.str["WIFI_SSID"], config.str["WIFI_PASSWORD"]);
  }

  beginTimeSync();

  if (!wifiConnected)
  {
    APManager::enableAccessPoint();
  }

  FirebaseManager::begin();

  display("L.E.E.F. Ready").clear().print();
  display("Watching plants").bottom().print();
  delay(2000);
}
void loop()
{
  connectRelayToMoistureSensor(*relay_R01, *moistureSensor_MM01);

  WiFiManager::maintainConnection();

  if (lastMoisturePercentage >= 0)
  {
    FirebaseManager::pushStatus(lastMoisturePercentage, lastMoistureTimestamp, relay_R01->getState(), lastRelayTimestamp);
  }

  FirebaseManager::checkForFirmwareUpdate();
  FirebaseManager::maintainPairing();

  if (WiFi.status() == WL_CONNECTED)
  {
    display::checkBacklight();
  }

  APManager::handlePortal();

  delay(1000);
}