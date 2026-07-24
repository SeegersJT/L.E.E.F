#include "watering/watering_controller.h"
#include "cloud/status_reporter.h"
#include "core/globals.h"
#include "core/time_utils.h"

WateringController::WateringController(DeviceWrapper<RelayDevice> &relay, DeviceWrapper<MoistureDevice> &moisture)
    : relay(relay), moisture(moisture), state(WateringState::IDLE), pulseCount(0), lastMoisture(-1) {}

int WateringController::lastMoisturePercentage() const
{
    return lastMoisture;
}

const String &WateringController::lastMoistureTimestamp() const
{
    return moistureTimestampValue;
}

const String &WateringController::lastRelayTimestamp() const
{
    return relayTimestampValue;
}

void WateringController::tick()
{
    unsigned long currentMillis = millis();

    switch (state)
    {
    case WateringState::IDLE:
        handleIdle(currentMillis);
        break;
    case WateringState::PULSE_ON:
        handlePulseOn(currentMillis);
        break;
    case WateringState::PULSE_SETTLE:
        handlePulseSettle(currentMillis);
        break;
    }
}

void WateringController::handleIdle(unsigned long currentMillis)
{
    if (currentMillis - moisture.getTimestamp() < (unsigned long)(config["MOISTURE_SENSORS_INTERVAL_MINUTES"] * 60 * 1000))
        return;

    int moisturePercentage = moisture.getMoisturePercentage();
    lastMoisture = moisturePercentage;
    moistureTimestampValue = currentIsoTimestamp();
    StatusReporter::logMoistureReading(moisturePercentage, moistureTimestampValue);
    display("Moisture: " + String(moisturePercentage) + "%").clear().print();

    if (moisturePercentage < config["ACTIVATE_RELAY_THRESHOLD"])
    {
        pulseCount = 0;
        relay.setState("ON");
        relayTimestampValue = currentIsoTimestamp();
        StatusReporter::logRelayEvent("ON", relayTimestampValue);
        display("Watering... (1)").bottom().print();
        state = WateringState::PULSE_ON;
    }
    else
    {
        display("Moisture OK").bottom().print();
    }
}

void WateringController::handlePulseOn(unsigned long currentMillis)
{
    if (currentMillis - relay.getTimestamp() < (unsigned long)config["RELAY_ON_DURATION"])
        return;

    relay.setState("OFF");
    relayTimestampValue = currentIsoTimestamp();
    StatusReporter::logRelayEvent("OFF", relayTimestampValue);
    pulseCount++;
    state = WateringState::PULSE_SETTLE;
}

void WateringController::handlePulseSettle(unsigned long currentMillis)
{
    if (currentMillis - relay.getTimestamp() < (unsigned long)config["PULSE_RECHECK_DELAY"])
        return;

    int moisturePercentage = moisture.getMoisturePercentage();
    lastMoisture = moisturePercentage;
    moistureTimestampValue = currentIsoTimestamp();
    StatusReporter::logMoistureReading(moisturePercentage, moistureTimestampValue);
    display("Moisture: " + String(moisturePercentage) + "%").clear().print();

    if (moisturePercentage >= config["ACTIVATE_RELAY_THRESHOLD"])
    {
        display("Watering done").bottom().print();
        state = WateringState::IDLE;
    }
    else if (pulseCount >= config["MAX_WATERING_PULSES"])
    {
        display("Pulse limit hit").bottom().print();
        state = WateringState::IDLE;
    }
    else
    {
        relay.setState("ON");
        relayTimestampValue = currentIsoTimestamp();
        StatusReporter::logRelayEvent("ON", relayTimestampValue);
        display("Watering... (" + String(pulseCount + 1) + ")").bottom().print();
        state = WateringState::PULSE_ON;
    }
}