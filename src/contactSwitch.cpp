#include "contactSwitch.h"
#include "config.h"

ContactSwitch::ContactSwitch(uint8_t endpointId, uint8_t pin, 
                             bool batteryMonitoring, uint8_t batteryPin,
                             uint32_t vDividerR1, uint32_t vDividerR2)
    : _zigbeeSwitch(endpointId),           // Initialize ZigbeeContactSwitch first
      ZbSensor(&_zigbeeSwitch)             // Pass pointer to superclass
{
    // Store endpoint ID and pin for later use
    _endpointId = endpointId;
    _pin = pin;
    
    // Optional: Setup battery monitoring if enabled
    if (batteryMonitoring && batteryPin != 255) {
        _batteryMonitoring = true;
        _batteryPin = batteryPin;
        _vDividerR1 = vDividerR1;
        _vDividerR2 = vDividerR2;
    } else {
        _batteryMonitoring = false;
    }

    // Initialize last reported state and change time for debouncing
    _lastReportedState = true; // Assume switch starts in open state
    _lastChangeTime = 0;
}

void ContactSwitch::setup()
{
    // Initialize contact switch pin
    pinMode(_pin, INPUT_PULLUP);
    // Optional: Set Zigbee device name and model
    // Add endpoint to Zigbee Core
    DEBUG_PRINTLN("Adding Zigbee endpoint to Zigbee Core");
    Zigbee.addEndpoint(&_zigbeeSwitch);
    // Optional: Setup battery monitoring if enabled
    if (_batteryMonitoring) {
	    // Init battery voltage pin
        pinMode(_batteryPin, INPUT);
	    _zigbeeSwitch.setPowerSource(ZB_POWER_SOURCE_BATTERY);
    }
}

void ContactSwitch::tick()
{
    bool reading = digitalRead(_pin);
    // Button is INPUT_PULLUP: HIGH = not pushed (open), LOW = pushed (closed)
    bool contactOpen = (reading == HIGH);

    // Only process if state changed AND debounce period has elapsed since last change
    if (contactOpen != _lastReportedState && (millis() - _lastChangeTime) > _debounceDelay) {
        _lastReportedState = contactOpen;
        _lastChangeTime = millis();
        
        // Report to Zigbee using correct methods
        if (contactOpen) {
        _zigbeeSwitch.setClosed();
        //digitalWrite(STATUS_LED_PIN, LOW);
        DEBUG_PRINTLN("Button is pressed.");
        } else {
        _zigbeeSwitch.setOpen();
        //digitalWrite(STATUS_LED_PIN, HIGH);
        DEBUG_PRINTLN("Button is released.");
        }
    }
}

void ContactSwitch::IASZoneEnrollment()
{
    DEBUG_PRINTLN("Enrolling IAS Zone...");
    if (_zigbeeSwitch.requestIASZoneEnroll()) {
        DEBUG_PRINTLN("IAS Zone enrollment requested successfully");
    } else {
        DEBUG_PRINTLN("IAS Zone enrollment request failed");
    }
}

bool ContactSwitch::getSwitchState()
{
    return digitalRead(_pin) == HIGH;
}