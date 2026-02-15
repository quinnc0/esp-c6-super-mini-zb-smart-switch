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
    // Constructor takes Zigbee endpoint ID and sensor PIN
    // Optional battery monitoring with voltage divider resistor values (in kΩ)
    ContactSwitch(uint8_t endpointId, uint8_t pin, 
                  bool batteryMonitoring = false, 
                  uint8_t batteryPin = 255,
                  uint32_t vDividerR1 = 0,
                  uint32_t vDividerR2 = 0);

    void setup() override;
    void tick() override;
    void switchWakeUp();
    void IASZoneEnrollment();
    bool getSwitchState();
};
