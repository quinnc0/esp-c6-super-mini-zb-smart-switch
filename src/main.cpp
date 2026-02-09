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

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include <Arduino.h>
#include "Zigbee.h"

/* Zigbee contact switch configuration */
#define ZIGBEE_TEST_ENDPOINT 10
#define CONTACT_SWITCH_PIN 3
#define STATUS_LED_PIN 15

ZigbeeContactSwitch zbContactSwitch = ZigbeeContactSwitch(ZIGBEE_TEST_ENDPOINT);

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
  zbContactSwitch.setManufacturerAndModel("Espressif", "ZBContactSwitch");

  // Add endpoint to Zigbee Core
  Serial.println("Adding Zigbee endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbContactSwitch);

  // When all EPs are registered, start Zigbee in End Device mode
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
  
  // Enroll the IAS Zone device with the coordinator
  Serial.println("Enrolling IAS Zone...");
  if (zbContactSwitch.requestIASZoneEnroll()) {
    Serial.println("IAS Zone enrollment requested successfully");
  } else {
    Serial.println("IAS Zone enrollment request failed");
  }
}

// Debounced contact switch handler - handles immediately, then ignores bounces
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
      zbContactSwitch.setOpen();
      digitalWrite(STATUS_LED_PIN, HIGH);
      Serial.println("Button is released.");
    } else {
      zbContactSwitch.setClosed();
      digitalWrite(STATUS_LED_PIN, LOW);
      Serial.println("Button is pressed.");
    }
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
}

