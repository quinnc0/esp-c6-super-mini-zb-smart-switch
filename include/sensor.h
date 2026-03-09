#pragma once
#include "ZigbeeEP.h"
#include "config.h"

// Sensor base class definition
// sensors will inherit from this class
class ZbSensor {
protected:
    // Common sensor properties can be added here (e.g. sensor ID, type)
    uint8_t _endpointId;
    uint8_t _pin;
    bool _batteryMonitoring;
    uint8_t _batteryPin;
    uint32_t _vDividerR1;  // Voltage divider R1 (kΩ) - between battery and pin
    uint32_t _vDividerR2;  // Voltage divider R2 (kΩ) - between pin and ground
    unsigned long _BATTERY_REPORT_INTERVAL = 10000; // interval in ms


    ZigbeeEP* _zigbeeEndpoint; // Pointer to Zigbee endpoint (subclass provides specific type)
public:
    // Constructor - subclasses must pass their specific endpoint
    ZbSensor(ZigbeeEP* endpoint) : _zigbeeEndpoint(endpoint), _batteryMonitoring(false), _batteryPin(0), _vDividerR1(0), _vDividerR2(0) {}
    
    virtual void setup() = 0;   // Initialize sensor
    virtual void tick() = 0;    // Main sensor logic
    
    void setManufacturerAndModel(const char* manufacturer, const char* model) {
        _zigbeeEndpoint->setManufacturerAndModel(manufacturer, model);
    }
    virtual void setupBatteryMonitoring(bool batteryMonitoring, uint8_t batteryPin) {
        if (batteryMonitoring) {
            _batteryMonitoring = true;
            _batteryPin = batteryPin;
            // Init battery voltage pin
            pinMode(_batteryPin, INPUT);
            _zigbeeEndpoint->setPowerSource(ZB_POWER_SOURCE_BATTERY);
        } else {
            _batteryMonitoring = false;
        }
    }

    // Method to read battery voltage and report to Zigbee
    virtual void reportBatteryStatus() {
        if (!_batteryMonitoring) return; // Skip if battery monitoring not enabled
        static bool firstRun = true;

        // Battery voltage variables
        static unsigned long lastBatteryReport = 0;
        if (firstRun || millis() - lastBatteryReport >= _BATTERY_REPORT_INTERVAL) {
            lastBatteryReport = millis();
            firstRun = false;
            
            // Add small delay and yield before analog read to ensure stability
            yield();
            delay(10);
            
            // Read voltage in millivolts (more accurate than ADC conversion)
            uint32_t pinMillivolts = analogReadMilliVolts(_batteryPin);
            
            // Voltage divider compensation: multiply by (R1 + R2) / R2
            uint32_t actualBatteryMillivolts = (pinMillivolts * (_vDividerR1 + _vDividerR2)) / _vDividerR2;
            
            // Convert to voltage for display
            float batteryVoltage = actualBatteryMillivolts / 1000.0;
            
            // Calculate battery percentage (for LiPo: 4.2V = 100%, 3.0V = 0%)
            const float MAX_VOLTAGE = 4.2;
            const float MIN_VOLTAGE = 3.0;
            float batteryPercent = getLiPoPercentage(batteryVoltage);
            
            // Report to Zigbee (voltage in millivolts and percentage)
            // Note: Zigbee battery voltage is typically reported in 100mV units
            uint8_t zigbeeBatteryVoltage = actualBatteryMillivolts / 100; // Convert mV to 100mV units
            uint8_t zigbeeBatteryPercent = (uint8_t)batteryPercent; // 0-100 scale (library handles Zigbee conversion)
                _zigbeeEndpoint->setBatteryVoltage(zigbeeBatteryVoltage);
                _zigbeeEndpoint->setBatteryPercentage(zigbeeBatteryPercent);
                _zigbeeEndpoint->reportBatteryPercentage();
            
            DEBUG_PRINTF("Battery: %.2fV (%dmV raw, %dmV actual) - %.1f%%\n", 
                        batteryVoltage, pinMillivolts, actualBatteryMillivolts, batteryPercent);
        }
    }

    // Use a more accurate percentage calculation for LiPo batteries based on voltage thresholds and non-linear discharge curve
    virtual float getLiPoPercentage(float voltage) {
        /*
        const float MAX_VOLTAGE = 4.2;
        const float MIN_VOLTAGE = 3.0;
        float percent = ((voltage - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE)) * 100.0;
        return constrain(percent, 0, 100);
        */
        if (voltage >= 4.1) return 100.0;
        else if (voltage >= 3.9) return 80.0 + (voltage - 3.9) * 100.0; // 3.9-4.1V = 80-100%
        else if (voltage >= 3.7) return 50.0 + (voltage - 3.7) * 150.0; // 3.7-3.9V = 50-80%
        else if (voltage >= 3.5) return 30.0 + (voltage - 3.5) * 100.0; // 3.5-3.7V = 30-50%
        else if (voltage >= 3.3) return 10.0 + (voltage - 3.3) * 100.0; // 3.3-3.5V = 10-30%
        else if (voltage >= 3.0) return (voltage - 3.0) * 33.3;          // 3.0-3.3V = 0-10%
        else return 0.0;
    }

    virtual void setBatteryReportInterval(unsigned long interval) {
        _BATTERY_REPORT_INTERVAL = interval;
    }

    virtual ~ZbSensor() {} // Virtual destructor for proper cleanup
};