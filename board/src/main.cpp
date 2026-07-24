#include <Arduino.h>
#include "core/globals.h"
#include "core/logger.h"
#include "core/time_utils.h"
#include "hardware/device_wrapper.h"
#include "network/wifi_manager.h"
#include "network/ap_manager.h"
#include "cloud/firebase_client.h"
#include "cloud/status_reporter.h"
#include "cloud/pairing_manager.h"
#include "cloud/ota_manager.h"
#include "watering/watering_controller.h"

// ============================================================
// DEVICES
// ============================================================

DeviceWrapper<MoistureDevice> *moistureSensor_MM01;
DeviceWrapper<RelayDevice> *relay_R01;
WateringController *wateringController;

// ============================================================
// STARTUP HELPERS
// ============================================================

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

    wateringController = new WateringController(*relay_R01, *moistureSensor_MM01);

    display("Device Setup").clear().print();
    display("Successful").bottom().print();
    delay(2000);
}

// ============================================================
// SETUP
// ============================================================

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

    FirebaseClient::begin(
        config.str["FIREBASE_API_KEY"], config.str["FIREBASE_DATABASE_URL"],
        config.str["FIREBASE_STORAGE_BUCKET"], config.str["FIREBASE_DEVICE_EMAIL"],
        config.str["FIREBASE_DEVICE_PASSWORD"]);

    display("L.E.E.F. Ready").clear().print();
    display("Watching plants").bottom().print();
    delay(2000);
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    wateringController->tick();

    WiFiManager::maintainConnection();

    if (wateringController->lastMoisturePercentage() >= 0)
    {
        StatusReporter::pushStatus(
            wateringController->lastMoisturePercentage(),
            wateringController->lastMoistureTimestamp(),
            relay_R01->getState(),
            wateringController->lastRelayTimestamp());
    }

    OtaManager::checkForUpdate();
    PairingManager::maintain();

    if (WiFi.status() == WL_CONNECTED)
    {
        display::checkBacklight();
    }

    APManager::handlePortal();

    delay(1000);
}