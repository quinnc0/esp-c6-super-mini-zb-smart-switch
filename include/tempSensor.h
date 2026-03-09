// Temperature sensor class definition
// Inherits from ZbSensor
#pragma once
#include "sensor.h"
#include <cstdint>
#include "Zigbee.h"
#include <DHT.h>

class TempSensor : public ZbSensor {
private:
    ZigbeeTempSensor _zigbeeTempSensor;
    DHT* _dht;  // DHT sensor object (for external temp/humidity sensor)
    uint32_t _delayMS;

    // Response tracking for retry mechanism
    uint8_t _dataToSend;      // Number of attributes waiting for confirmation
    bool _resend;             // Flag to trigger immediate resend on failure
    unsigned long _lastReportTime;
    unsigned long _reportInterval;  // Interval between temperature reports (ms)

    // Static instance pointer for callback (assuming single temp sensor)
    static TempSensor* _instance;

    void _getReadings(float& temp, float& humidity);
    // Response callback handler
    void _handleResponse(zb_cmd_type_t command, esp_zb_zcl_status_t status);
    
    // Static callback wrapper for C-style function pointer
    static void _responseCallback(zb_cmd_type_t command, esp_zb_zcl_status_t status);

    // Report temperature with retry mechanism
    bool _reportWithRetry(uint32_t timeout = 1000, uint8_t maxRetries = 3);

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

    void reportReadings(); // Report sensor readings to Zigbee  
    
    // Set report interval (default: 60000ms = 1 minute)
    void setReportInterval(unsigned long intervalMs);

};