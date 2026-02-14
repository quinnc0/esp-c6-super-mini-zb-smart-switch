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
#include "config.h"
#include "sensor.h"
#include "contactSwitch.h"

ContactSwitch smartSwitch = ContactSwitch(SMART_SWITCH, CONTACT_SWITCH_PIN, BATTERY_ENABLED, BATTERY_VOLTAGE_PIN, V_DIVIDER_R1, V_DIVIDER_R2);

void setupZigbee();
void rgbLed(bool on);

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
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
  smartSwitch.setBatteryReportInterval(BATTERY_CHECK_INTERVAL);

  // Start Zigbee and connect to network
  setupZigbee();
  
  smartSwitch.IASZoneEnrollment();
}

void loop() {

  // Read contact switch state and report to Zigbee
  smartSwitch.tick();
  // Optional: Read battery voltage and report to Zigbee
  smartSwitch.reportBatteryStatus();

  // Update status LED based on switch state
  if (smartSwitch.getSwitchState()) {
    digitalWrite(STATUS_LED_PIN, LOW); // LED on when switch is closed
    rgbLed(false); // RGB LED on when switch is closed
  } else {
    digitalWrite(STATUS_LED_PIN, HIGH); // LED off when switch is open
    rgbLed(true); // RGB LED off when switch is open
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

// Control RGB LED
void rgbLed(bool on) {
  uint8_t r = on ? 255 : 0;
  uint8_t g = on ? 0 : 0;
  uint8_t b = on ? 10 : 0;
  uint8_t brightness = 255; // Adjust brightness (0-255)
  rgbLedWrite(RGB_BUILTIN, r, g, b);
}