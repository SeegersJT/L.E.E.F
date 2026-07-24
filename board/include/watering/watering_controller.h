#ifndef WATERING_CONTROLLER_H
#define WATERING_CONTROLLER_H

#include <Arduino.h>
#include "hardware/device_wrapper.h"

class WateringController
{
public:
    WateringController(DeviceWrapper<RelayDevice> &relay, DeviceWrapper<MoistureDevice> &moisture);

    void tick();

    int lastMoisturePercentage() const;
    const String &lastMoistureTimestamp() const;
    const String &lastRelayTimestamp() const;

private:
    DeviceWrapper<RelayDevice> &relay;
    DeviceWrapper<MoistureDevice> &moisture;

    WateringState state;
    int pulseCount;
    int lastMoisture;
    String moistureTimestampValue;
    String relayTimestampValue;

    void handleIdle(unsigned long currentMillis);
    void handlePulseOn(unsigned long currentMillis);
    void handlePulseSettle(unsigned long currentMillis);
};

#endif // WATERING_CONTROLLER_H