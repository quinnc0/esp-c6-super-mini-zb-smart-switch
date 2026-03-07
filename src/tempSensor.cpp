#include "tempSensor.h"
#include "config.h"

TempSensor::TempSensor(uint8_t endpointId, uint8_t pin,
                       bool batteryMonitoring, uint8_t batteryPin,
                       uint32_t vDividerR1, uint32_t vDividerR2)
    : _zigbeeTempSensor(endpointId), // Initialize ZigbeeTempSensor first
      ZbSensor(&_zigbeeTempSensor)   // Pass pointer to superclass
{
    // Store endpoint ID and pin for later use
    _endpointId = endpointId;
    _pin = pin;
    _dht = new DHT_Unified(_pin, DHT22);
    // Optional: Setup battery monitoring if enabled
    if (batteryMonitoring && batteryPin != 255) {
        _batteryMonitoring = true;
        _batteryPin = batteryPin;
        _vDividerR1 = vDividerR1;
        _vDividerR2 = vDividerR2;
    } else {
        _batteryMonitoring = false;
    }
    
    // Initialize response tracking
    _dataToSend = 0;
    _resend = false;
    _lastReportTime = 0;
    _reportInterval = 60000;  // Default: report every 60 seconds
}

void TempSensor::setup()
{
    _zigbeeTempSensor.setMinMaxValue(-40, 80);
    _zigbeeTempSensor.setTolerance(0.5);
    _zigbeeTempSensor.addHumiditySensor(0, 100, 2, 30);

    Zigbee.addEndpoint(&_zigbeeTempSensor);

    // Optional: Setup battery monitoring if enabled
    if (_batteryMonitoring) {
        // Init battery voltage pin
        pinMode(_batteryPin, INPUT);
        _zigbeeTempSensor.setPowerSource(ZB_POWER_SOURCE_BATTERY);
    }

    // DHT sensor initialization
    _dht->begin();
    sensor_t sensor;
    _dht->temperature().getSensor(&sensor);
    _dht->humidity().getSensor(&sensor);
    _delayMS = sensor.min_delay / 1000;
}

void TempSensor::tick()
{
    // Check if it's time to report temperature
    if (millis() - _lastReportTime >= _reportInterval) {
        _lastReportTime = millis();
        
        DEBUG_PRINTLN("Reading temperature sensor...");
        sensors_event_t readings = _getReadings();
    
        float temperature = readings.temperature;
        float humidity = readings.relative_humidity;
        
        DEBUG_PRINTF("Temperature: %.2f°C, Humidity: %.2f%%\n", temperature, humidity);
        
        // Update values in Zigbee endpoint
        _zigbeeTempSensor.setTemperature(temperature);
        _zigbeeTempSensor.setHumidity(humidity);
        
        // Report with retry mechanism
        reportWithRetry(1000, 3);
    }
    
    // Report battery status if enabled
    reportBatteryStatus();
}

sensors_event_t TempSensor::_getReadings()
{
    // Get temperature event and print its value.
    sensors_event_t event;
    _dht->temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        DEBUG_PRINTLN("Error reading temperature!");
    } else {
        DEBUG_PRINTF("Temperature: %.2f°C\n", event.temperature);
    }
    _dht->humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        DEBUG_PRINTLN("Error reading humidity!");
    } else {
        DEBUG_PRINTF("Humidity: %.2f%%\n", event.relative_humidity);
    }
    return event;
}

void TempSensor::handleResponse(uint8_t command, uint8_t status, uint8_t endpoint, uint16_t cluster)
{
    // Only handle responses for this endpoint
    if (endpoint != _endpointId) {
        return;
    }
    
    // Only handle attribute report responses
    if (command != 0x01) {  // ZB_CMD_REPORT_ATTRIBUTE = 0x01
        return;
    }
    
    DEBUG_PRINTF("TempSensor response - Status: %d, Endpoint: %d, Cluster: 0x%04x\n", status, endpoint, cluster);
    
    switch (status) {
        case 0x00:  // ESP_ZB_ZCL_STATUS_SUCCESS
            _dataToSend--;
            DEBUG_PRINTF("Report confirmed! Remaining: %d\n", _dataToSend);
            break;
        case 0x01:  // ESP_ZB_ZCL_STATUS_FAIL
            _resend = true;
            DEBUG_PRINTLN("Report failed! Will resend.");
            break;
        default:
            // Handle other statuses (timeout, invalid value, etc.)
            DEBUG_PRINTF("Report status: %d\n", status);
            break;
    }
}

void TempSensor::setReportInterval(unsigned long intervalMs)
{
    _reportInterval = intervalMs;
}

bool TempSensor::reportWithRetry(uint32_t timeout, uint8_t maxRetries)
{
    // Set data counter (2 for temp + humidity)
    _dataToSend = 2;
    _resend = false;
    
    // Send initial report
    DEBUG_PRINTLN("Reporting temperature and humidity...");
    _zigbeeTempSensor.report();
    
    unsigned long startTime = millis();
    uint8_t tries = 0;
    
    // Wait for confirmation with retry logic
    while (_dataToSend != 0 && tries < maxRetries) {
        // Check for immediate resend on failure
        if (_resend) {
            DEBUG_PRINTLN("Resending on failure!");
            _resend = false;
            _dataToSend = 2;
            _zigbeeTempSensor.report();
            startTime = millis();
            tries++;
        }
        
        // Check for timeout
        if (millis() - startTime >= timeout) {
            DEBUG_PRINTLN("Report timeout! Retrying...");
            _dataToSend = 2;
            _zigbeeTempSensor.report();
            startTime = millis();
            tries++;
        }
        
        delay(50);  // Small delay to avoid busy-waiting
    }
    
    if (_dataToSend == 0) {
        DEBUG_PRINTLN("Temperature report confirmed successfully!");
        return true;
    } else {
        DEBUG_PRINTF("Temperature report failed after %d retries\n", tries);
        return false;
    }
}
