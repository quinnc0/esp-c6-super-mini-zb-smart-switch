// Temperature sensor class definition
// Inherits from ZbSensor
#pragma once
#include "sensor.h"
#include <cstdint>
#include "Zigbee.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

class TempSensor : public ZbSensor {
private:
    ZigbeeTempSensor _zigbeeTempSensor;
    DHT_Unified* _dht;  // DHT sensor object (for external temp/humidity sensor)
    uint32_t _delayMS;

    // Response tracking for retry mechanism
    uint8_t _dataToSend;      // Number of attributes waiting for confirmation
    bool _resend;             // Flag to trigger immediate resend on failure
    unsigned long _lastReportTime;
    unsigned long _reportInterval;  // Interval between temperature reports (ms)

    sensors_event_t _getReadings();

public:
    // Constructor takes Zigbee endpoint ID
    // Optional battery monitoring with voltage divider resistor values (in kΩ)
    TempSensor(uint8_t endpointId, uint8_t pin,
               bool batteryMonitoring = false, 
               uint8_t batteryPin = 255,
               uint32_t vDividerR1 = 0,
               uint32_t vDividerR2 = 0);

    void setup() override;
    void tick() override;
    
    // Response callback handler
    void handleResponse(uint8_t command, uint8_t status, uint8_t endpoint, uint16_t cluster);
    
    // Set report interval (default: 60000ms = 1 minute)
    void setReportInterval(unsigned long intervalMs);
    
    // Report temperature with retry mechanism
    bool reportWithRetry(uint32_t timeout = 1000, uint8_t maxRetries = 3);

};