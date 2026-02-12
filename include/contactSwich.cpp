#include "contactSwich.h"

ContactSwitch::ContactSwitch(uint8_t endpointId, uint8_t pin, bool batteryMonitoring, uint8_t batteryPin)
{
    // Store endpoint ID and pin for later use
    _endpointId = endpointId;
    _pin = pin;
    // Initialize ZigbeeContactSwitch with the endpoint ID
    _zigbeeSwitch = ZigbeeContactSwitch(_endpointId);
}

void ContactSwitch::setup()
{
}

void ContactSwitch::tick()
{
}

void ContactSwitch::setManufacturerAndModel(const char *manufacturer, const char *model)
{
}

void ContactSwitch::setupBatteryMonitoring()
{
}
