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
#include "sensor.h"
#include "contactSwitch.h"

#define BATTERY_ENABLED false  // Set to true if battery monitoring is needed

// PIN definitions
#define CONTACT_SWITCH_PIN 3
#define STATUS_LED_PIN 15
#define BATTERY_VOLTAGE_PIN 2
// Zigbee endpoints
#define ZIGBEE_TEST_ENDPOINT 10
// Voltage divider resistors for battery voltage measurement (in kΩ)
// Adjust these values based on your actual resistor values
const uint32_t V_DIVIDER_R1 = 55; //39; // Resistor between battery and pin
const uint32_t V_DIVIDER_R2 = 99; //100; // Resistor between pin and ground

ContactSwitch smartSwitch = ContactSwitch(ZIGBEE_TEST_ENDPOINT, CONTACT_SWITCH_PIN, BATTERY_ENABLED, BATTERY_VOLTAGE_PIN, V_DIVIDER_R1, V_DIVIDER_R2);

void setupZigbee();

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("CONTACT SWITCH ZIGBEE TEST - PlatformIO");
  Serial.println("========================================\n");

  // Init status LED pin
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  // Optional: Set Zigbee device name and model
  smartSwitch.setManufacturerAndModel("Super Mini", "Smart Switch");

  // Add endpoints to Zigbee Core
  smartSwitch.setup();

  // Start Zigbee and connect to network
  setupZigbee();
  
  smartSwitch.IASZoneEnrollment();
}

void loop() {
  // Print status every 5 seconds
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    Serial.println("Alive... Zigbee connected: " + String(Zigbee.connected() ? "YES" : "NO"));
    lastPrint = millis();
  }

  smartSwitch.tick();
  smartSwitch.reportBatteryStatus();
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