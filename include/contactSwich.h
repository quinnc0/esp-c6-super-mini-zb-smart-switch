// Contact switch sensor class definition
// Inherits from ZbSensor
#pragma once
#include "sensor.h"
#include <cstdint>
#include "Zigbee.h"

class ContactSwitch : public ZbSensor {
private:
    uint8_t _endpointId;
    uint8_t _pin;
    ZigbeeContactSwitch _zigbeeSwitch;
public:
    //constructor takes Zigbee endpoint ID and sensor PIN
    ContactSwitch(uint8_t endpointId, uint8_t pin, bool batteryMonitoring = false, uint8_t batteryPin = -1);

    void setup() override;
    void tick() override;
    void setManufacturerAndModel(const char* manufacturer, const char* model) override;
    void setupBatteryMonitoring() override;
};
