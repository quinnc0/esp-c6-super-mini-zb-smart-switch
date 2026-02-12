// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @brief MINIMAL ZIGBEE TEST for ESP32C6 on PlatformIO
 * 
 * This is a simplified version of the Arduino IDE RGB example to test
 * if Zigbee initialization works on PlatformIO with pioarduino platform.
 * 
 * Based on: Arduino ESP32 Zigbee ColorDimmableLight example
 */

#include <Arduino.h>
#include "Zigbee.h"

#define BATTERY_ENABLED false  // Set to true if battery monitoring is needed

// PIN definitions
#define CONTACT_SWITCH_PIN 3
#define STATUS_LED_PIN 15
#define BATTERY_VOLTAGE_PIN 2
// Zigbee endpoints
#define ZIGBEE_TEST_ENDPOINT 10
#define BATTERY_VOLTAGE_ENDPOINT 4
// Voltage divider resistors for battery voltage measurement (in kΩ)
// Adjust these values based on your actual resistor values
const uint32_t V_DIVIDER_R1 = 55; //39; // Resistor between battery and pin
const uint32_t V_DIVIDER_R2 = 99; //100; // Resistor between pin and ground

ZigbeeContactSwitch zbContactSwitch = ZigbeeContactSwitch(ZIGBEE_TEST_ENDPOINT);

void setupZigbee();
void handleContactSwitch();
void setupBatteryMonitoring(ZigbeeEP *zbEndPoint);
void handleBatteryVoltage();
float getLiPoPercentage(float voltage);

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("CONTACT SWITCH ZIGBEE TEST - PlatformIO");
  Serial.println("========================================\n");

  // Init contact switch pin
  pinMode(CONTACT_SWITCH_PIN, INPUT_PULLUP);
  // Init status LED pin
  pinMode(STATUS_LED_PIN, OUTPUT);
  

  // Optional: Set Zigbee device name and model
  zbContactSwitch.setManufacturerAndModel("Super Mini", "Smart Switch");

  // Add endpoints to Zigbee Core
  Serial.println("Adding Zigbee endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbContactSwitch);

  if (BATTERY_ENABLED) {
		// Setup battery monitoring
		Serial.println("Setting up battery monitoring...");
		setupBatteryMonitoring(&zbContactSwitch);
	}

  // Start Zigbee and connect to network
  setupZigbee();
  
  // Enroll the IAS Zone device with the coordinator
  Serial.println("Enrolling IAS Zone...");
  if (zbContactSwitch.requestIASZoneEnroll()) {
    Serial.println("IAS Zone enrollment requested successfully");
  } else {
    Serial.println("IAS Zone enrollment request failed");
  }
}

void loop() {
  // Print status every 5 seconds
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    Serial.println("Alive... Zigbee connected: " + String(Zigbee.connected() ? "YES" : "NO"));
    lastPrint = millis();
  }

  handleContactSwitch();

  if (BATTERY_ENABLED) {
  	handleBatteryVoltage();
  }
}

//===============================================================================//
//------------------------------- Helper functions ------------------------------//
//===============================================================================//

// Zigbee setup function - initializes Zigbee and waits for connection
void setupZigbee() {
        Serial.println("Starting Zigbee...");
        if (!Zigbee.begin()) {
            Serial.println("Zigbee failed to start!");
            Serial.println("Rebooting...");
            ESP.restart();
        }
        
        Serial.println("Zigbee started successfully!");
        Serial.println("Connecting to network");
        
        while (!Zigbee.connected()) {
            Serial.print(".");
            delay(100);
        }
        
        Serial.println();
        Serial.println("\n========================================");
        Serial.println("SUCCESS! Zigbee connected!");
        Serial.println("========================================\n");
        return;
}

// Debounced contact switch handler - reacts immediately, then ignores bounces
void handleContactSwitch() {
  static bool lastReportedState = true; // true = open (not pushed), false = closed (pushed)
  static unsigned long lastChangeTime = 0;
  const unsigned long debounceDelay = 100;  // Increased to give Zigbee/HA time to process

  bool reading = digitalRead(CONTACT_SWITCH_PIN);
  // Button is INPUT_PULLUP: HIGH = not pushed (open), LOW = pushed (closed)
  bool contactOpen = (reading == HIGH);

  // Only process if state changed AND debounce period has elapsed since last change
  if (contactOpen != lastReportedState && (millis() - lastChangeTime) > debounceDelay) {
    lastReportedState = contactOpen;
    lastChangeTime = millis();
    
    // Report to Zigbee using correct methods
    if (contactOpen) {
      zbContactSwitch.setClosed();
      digitalWrite(STATUS_LED_PIN, LOW);
      Serial.println("Button is pressed.");
    } else {
      zbContactSwitch.setOpen();
      digitalWrite(STATUS_LED_PIN, HIGH);
      Serial.println("Button is released.");
    }
  }
}

// Battery monitoring setup - can be called in setup() to initialize any necessary variables or states
// inputs - pointer to ZigbeeEP object to report battery status to
void setupBatteryMonitoring(ZigbeeEP *zbEndPoint) {
	// Init battery voltage pin
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);
	zbEndPoint->setPowerSource(ZB_POWER_SOURCE_BATTERY);
}

// Battery voltage handler - reads voltage using analogReadMilliVolts and reports periodically
void handleBatteryVoltage() {
	// Battery voltage variables
	static unsigned long lastBatteryReport = 0;
	const unsigned long BATTERY_REPORT_INTERVAL = 10000; // Report every 60 seconds
  if (millis() - lastBatteryReport >= BATTERY_REPORT_INTERVAL) {
		lastBatteryReport = millis();
    
    // Read voltage in millivolts (more accurate than ADC conversion)
    uint32_t pinMillivolts = analogReadMilliVolts(BATTERY_VOLTAGE_PIN);
    
    // Voltage divider compensation: multiply by (R1 + R2) / R2
    // Example: For 39kΩ + 100kΩ divider: multiply by 1.39 (or 139/100)
    uint32_t actualBatteryMillivolts = (pinMillivolts * (V_DIVIDER_R1 + V_DIVIDER_R2)) / V_DIVIDER_R2;
    
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
		zbContactSwitch.setBatteryVoltage(zigbeeBatteryVoltage);
		zbContactSwitch.setBatteryPercentage(zigbeeBatteryPercent);
		zbContactSwitch.reportBatteryPercentage();
    
    Serial.printf("Battery: %.2fV (%dmV raw, %dmV actual) - %.1f%%\n", 
                  batteryVoltage, pinMillivolts, actualBatteryMillivolts, batteryPercent);
  }
}

// Use a more accurate percentage calculation for LiPo batteries based on voltage thresholds and non-linear discharge curve
float getLiPoPercentage(float voltage) {
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