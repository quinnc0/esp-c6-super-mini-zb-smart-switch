// Contact switch sensor class definition
// Inherits from ZbSensor
#pragma once
#include "sensor.h"
#include <cstdint>
#include "Zigbee.h"

class ContactSwitch : public ZbSensor {
private:

    ZigbeeContactSwitch _zigbeeSwitch;

    bool _lastReportedState;
    unsigned long _lastChangeTime;
    const unsigned long _debounceDelay = 100;  // Increased to give Zigbee/HA time to process
public:
    //constructor takes Zigbee endpoint ID and sensor PIN
    ContactSwitch(uint8_t endpointId, uint8_t pin, bool batteryMonitoring = false, uint8_t batteryPin = -1);

    void setup() override;
    void tick() override;
    void IASZoneEnrollment();
};
