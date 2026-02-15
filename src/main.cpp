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
#include "esp_sleep.h"
#include "driver/rtc_io.h"

ContactSwitch contactSwitch = ContactSwitch(SMART_SWITCH, CONTACT_SWITCH_PIN, BATTERY_ENABLED, BATTERY_VOLTAGE_PIN, V_DIVIDER_R1, V_DIVIDER_R2);

// Sleep configuration
unsigned long loopStartTime = 0;
bool sleepTimerStarted = false;

void setupZigbee();
void rgbLed(bool on);
void goToSleep();
esp_sleep_wakeup_cause_t onWakeCheck();
void handleWakeUp(esp_sleep_wakeup_cause_t wakeupReason);

//===============================================================================//
//------------------------------- Setup -----------------------------------------//
//===============================================================================//
void setup() {
  #if ENABLE_SERIAL
    Serial.begin(SERIAL_BAUD_RATE);
    delay(50); // Short delay to ensure serial is ready before printing
  #endif

  DEBUG_PRINTLN("CONTACT SWITCH ZIGBEE - PlatformIO");
  esp_sleep_wakeup_cause_t wakeupReason = onWakeCheck(); // Check if we woke from deep sleep and handle RTC GPIO if needed

  // Init status LED pin
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  // Optional: Set Zigbee device name and model
  contactSwitch.setManufacturerAndModel("Super Mini", "Smart Switch");

  // Add endpoints to Zigbee Core
  contactSwitch.setup();
  contactSwitch.setBatteryReportInterval(BATTERY_CHECK_INTERVAL);

  // Start Zigbee and connect to network
  setupZigbee();
  
  contactSwitch.IASZoneEnrollment();
  
  digitalWrite(STATUS_LED_PIN, HIGH);
  handleWakeUp(wakeupReason); // Handle wake-up actions after Zigbee is ready
}

//===============================================================================//
//------------------------------- Main Loop -------------------------------------//
//===============================================================================//

void loop() {
  // Start sleep timer on first loop iteration
  if (!sleepTimerStarted) {
    loopStartTime = millis();
    sleepTimerStarted = true;
  }

  // Read contact switch state and report to Zigbee
  contactSwitch.tick();
  // Optional: Read battery voltage and report to Zigbee
  contactSwitch.reportBatteryStatus();

  // Update status LED based on switch state
  if (contactSwitch.getSwitchState()) {
    rgbLed(false); // RGB LED on when switch is closed
    // reset sleep timer when switch is closed to keep device awake while in use
  } else {
    rgbLed(true); // RGB LED off when switch is open
    loopStartTime = millis();
  }
  
  // Check if it's time to sleep
  if (millis() - loopStartTime >= SLEEP_DELAY_MS) {
    goToSleep();
  }
}

//===============================================================================//
//------------------------------- Helper functions ------------------------------//
//===============================================================================//

// Zigbee setup function - initializes Zigbee and waits for connection
void setupZigbee() {
        DEBUG_PRINTLN("Starting Zigbee...");
        if (!Zigbee.begin()) {
            DEBUG_PRINTLN("Zigbee failed to start!");
            DEBUG_PRINTLN("Rebooting...");
            ESP.restart();
        }
        
        DEBUG_PRINTLN("Zigbee started successfully!");
        DEBUG_PRINTLN("Connecting to network");
        
        while (!Zigbee.connected()) {
            DEBUG_PRINT(".");
            delay(100);
        }
        
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("SUCCESS! Zigbee connected!");
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

// Check wake-up cause and handle RTC GPIO if woke from deep sleep
esp_sleep_wakeup_cause_t onWakeCheck() {
  #if !BATTERY_ENABLED
    // Wake-up handling only makes sense with battery monitoring
    return ESP_SLEEP_WAKEUP_UNDEFINED;
  #endif

  // Check if we woke from deep sleep and deinit RTC GPIO if needed
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    DEBUG_PRINTLN("Woke up from button press!");
    // Deinitialize RTC GPIO so pinMode can reconfigure it
    rtc_gpio_deinit((gpio_num_t)CONTACT_SWITCH_PIN);
  } else if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
    DEBUG_PRINTLN("Woke up from timer!");
    rtc_gpio_deinit((gpio_num_t)CONTACT_SWITCH_PIN);
  } else {
    DEBUG_PRINTLN("Normal startup (not from deep sleep)");
  }
  return wakeup_reason;
}

// Handle wake up from deep sleep and check wake-up cause
void handleWakeUp(esp_sleep_wakeup_cause_t wakeupReason) {
  #if !BATTERY_ENABLED
    // Wake-up handling only makes sense with battery monitoring
    return;
  #endif

  // Use wake reason after Zigbee is ready
  switch (wakeupReason) {
    case ESP_SLEEP_WAKEUP_EXT1:
      DEBUG_PRINTLN("Action: Woke from button press");
      // Give Zigbee a moment to stabilize after connection
      delay(200);
      // Read actual pin state and report to Zigbee
      contactSwitch.switchWakeUp();
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      DEBUG_PRINTLN("Action: Woke from timer");
      break;
    case ESP_SLEEP_WAKEUP_UNDEFINED:
    default:
      DEBUG_PRINTLN("Action: Normal startup");
      // First boot or reset
      break;
  }
}
// Put device into deep sleep
void goToSleep() {
  #if !BATTERY_ENABLED
    // Sleep mode only makes sense with battery monitoring
    return;
  #endif
  
  DEBUG_PRINTLN("Configuring wake-up sources...");
  
  // Configure RTC GPIO for wake-up
  // ESP32-C6 RTC GPIO pins: GPIO0-GPIO7
  gpio_num_t wakeup_pin = (gpio_num_t)CONTACT_SWITCH_PIN;
  
  // Isolate the pin for RTC use
  rtc_gpio_init(wakeup_pin);
  rtc_gpio_set_direction(wakeup_pin, RTC_GPIO_MODE_INPUT_ONLY);
  
  // Configure pull resistors for wake-up pin:
  rtc_gpio_pulldown_dis(wakeup_pin);  // Disable pull-down
  rtc_gpio_pullup_en(wakeup_pin);     // Enable pull-up
  
  // Configure EXT1 wake-up (ESP32-C6 only supports EXT1, not EXT0)
  uint64_t ext1_pin_mask = (1ULL << CONTACT_SWITCH_PIN);
  
  // ESP_EXT1_WAKEUP_ANY_LOW: wake when pin goes LOW (for pull-up button)
  esp_sleep_enable_ext1_wakeup(ext1_pin_mask, ESP_EXT1_WAKEUP_ANY_LOW);
  
  DEBUG_PRINTLN("Wake on button press enabled (EXT1 with pull-up)");
  
  // Enable timer wake-up for periodic battery checks
  esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_SECONDS * 1000000ULL);
  DEBUG_PRINTF("Timer wake-up enabled (every %d seconds)\n", TIMER_WAKEUP_SECONDS);
  
  // Turn off RGB LED to save power
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
  
  DEBUG_PRINTLN("Entering deep sleep now...");
  DEBUG_FLUSH(); // Wait for serial to finish
  
  // Enter deep sleep
  digitalWrite(STATUS_LED_PIN, LOW);
  esp_deep_sleep_start();
}