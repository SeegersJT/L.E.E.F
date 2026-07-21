#include <Arduino.h>
#include "wifi_manager.h"
#include "ap_manager.h"
#include "firebase_manager.h"
#include "globals.h"
#include "device_wrapper.h"

DeviceWrapper<MoistureDevice>* moistureSensor_MM01;
DeviceWrapper<RelayDevice>* relay_R01;

enum class WateringState { IDLE, PULSE_ON, PULSE_SETTLE };

WateringState wateringState = WateringState::IDLE;
int wateringPulseCount = 0;

int lastMoisturePercentage = -1;

void listSPIFFSFiles() {
  Serial.println("Listing SPIFFS files:");
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file) {
    Serial.print("FILE: ");
    Serial.print(file.name());
    Serial.print(" - SIZE: ");
    Serial.println(file.size());
    file = root.openNextFile();
  }

  Serial.println("");
}

// =========================================================
// START UP
// =========================================================

void startDisplay() {
  display::init();
  display("Starting...").print();
  delay(5000);
}

void setupDevices() {
  display("Device Setup").clear().print();
  display("Initiating...").bottom().print();
  delay(2000);

  moistureSensor_MM01 = new DeviceWrapper<MoistureDevice>(config["MOISTURE_SENSOR_PIN_MM01"], "MOISTURE SENSOR 01");
  relay_R01 = new DeviceWrapper<RelayDevice>(config["RELAY_PIN_R01"], "RELAY 01");

  moistureSensor_MM01->setup();
  relay_R01->setup();

  display("Device Setup").clear().print();
  display("Successfull").bottom().print();
  delay(2000);
}

// =========================================================
// CONNECTORS - Logic to control relay based on moisture sensor
// =========================================================

void connectRelayToMoistureSensor(DeviceWrapper<RelayDevice>& relayDevice, DeviceWrapper<MoistureDevice>& moistureDevice) {
  unsigned long currentMillis = millis();

  switch (wateringState) {

    case WateringState::IDLE: {
      if (currentMillis - moistureDevice.getTimestamp() >= config["MOISTURE_SENSORS_INTERVAL_MINUTES"] * 60 * 1000) {
        int moisturePercentage = moistureDevice.getMoisturePercentage();
        lastMoisturePercentage = moisturePercentage;
        display("Moisture: " + String(moisturePercentage) + "%").clear().print();

        if (moisturePercentage < config["ACTIVATE_RELAY_THRESHOLD"]) {
          wateringPulseCount = 0;
          relayDevice.setState("ON");
          display("Watering... (1)").bottom().print();
          wateringState = WateringState::PULSE_ON;
        } else {
          display("Moisture OK").bottom().print();
        }
      }
      break;
    }

    case WateringState::PULSE_ON: {
      if (currentMillis - relayDevice.getTimestamp() >= config["RELAY_ON_DURATION"]) {
        relayDevice.setState("OFF");
        wateringPulseCount++;
        wateringState = WateringState::PULSE_SETTLE;
      }
      break;
    }

    case WateringState::PULSE_SETTLE: {
      if (currentMillis - relayDevice.getTimestamp() >= config["PULSE_RECHECK_DELAY"]) {
        int moisturePercentage = moistureDevice.getMoisturePercentage();
        lastMoisturePercentage = moisturePercentage;
        display("Moisture: " + String(moisturePercentage) + "%").clear().print();

        if (moisturePercentage >= config["ACTIVATE_RELAY_THRESHOLD"]) {
          display("Watering done").bottom().print();
          wateringState = WateringState::IDLE;
        } else if (wateringPulseCount >= config["MAX_WATERING_PULSES"]) {
          display("Pulse limit hit").bottom().print();
          wateringState = WateringState::IDLE;
        } else {
          relayDevice.setState("ON");
          display("Watering... (" + String(wateringPulseCount + 1) + ")").bottom().print();
          wateringState = WateringState::PULSE_ON;
        }
      }
      break;
    }
  }
}

// =========================================================
// SETUP - Executed code at the start of the run
// =========================================================

void setup() {
  Serial.begin(9600);

  startDisplay();

  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  listSPIFFSFiles();

  config.initialConfig();
  config.readFromINI();
  config.readStringsFromINI("/config/wifi.ini");
  config.readStringsFromINI("/config/firebase.ini");

  setupDevices();

  WiFiManager::setHostname();

  bool wifiConnected = WiFiManager::connectToWiFi();

  if (!wifiConnected) {
    APManager::enableAccessPoint();
  }

  FirebaseManager::begin();
}

// =========================================================
// LOOP - Executed code in ipAddress loop
// =========================================================

void loop() {
  connectRelayToMoistureSensor(*relay_R01, *moistureSensor_MM01);

  WiFiManager::maintainConnection();

  if (lastMoisturePercentage >= 0) {
    FirebaseManager::pushStatus(lastMoisturePercentage, relay_R01->getState());
  }

  display::checkBacklight();

  APManager::handlePortal();

  delay(1000);
}