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
#include "tempSensor.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"

TempSensor tempSensor = TempSensor(TEMP_SENSOR_ENDPOINT, TEMP_SENSOR_PIN, BATTERY_ENABLED, BATTERY_VOLTAGE_PIN, V_DIVIDER_R1, V_DIVIDER_R2);

// Sleep configuration
unsigned long loopStartTime = 0;
bool sleepTimerStarted = false;

void setupZigbee();
void rgbLed(bool on);
void goToSleep();
void blinkLed(uint8_t pin, uint8_t count, uint16_t onMs, uint16_t offMs);
void factoryReset(uint8_t button);

//===============================================================================//
//------------------------------- Setup -----------------------------------------//
//===============================================================================//
void setup() {
  rgbLed(false); // Ensure RGB LED is off at startup
  #if ENABLE_SERIAL
    Serial.begin(SERIAL_BAUD_RATE);
    delay(50); // Short delay to ensure serial is ready before printing
  #endif

  DEBUG_PRINTLN("\n\n=================================");
  DEBUG_PRINTLN("TEMP SENSOR ZIGBEE - Booting...");
  DEBUG_PRINTLN("=================================");

  // Init status LED pin
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  // TEMPORARY: Force factory reset to clear old TMP36 identity from NVS
  // Uncomment this ONCE, flash, let it run, then comment it out again
  // Zigbee.factoryReset();
  // delay(3000);
  // DEBUG_PRINTLN("Factory reset complete! Now reflash with this commented out.");
  
  // Optional: Set Zigbee device name and model
  tempSensor.setManufacturerAndModel("Super Mini", "Drybox Humidity");

  // Add endpoints to Zigbee Core
  tempSensor.setup();
  tempSensor.setBatteryReportInterval(BATTERY_CHECK_INTERVAL);

  // Start Zigbee and connect to network
  setupZigbee();
  
  // Blink 2 times: Zigbee connected
  blinkLed(STATUS_LED_PIN, 2, 200, 200);

  // report to zigbee, then go to sleep if battery monitoring enabled
  if (BATTERY_ENABLED) {
    delay(2000); // Wait 2 seconds for network to stabilize
    
    // Read temperature sensor state and report to Zigbee
    rgbLed(true); // Turn on RGB LED to indicate activity
    tempSensor.reportReadings();
    blinkLed(STATUS_LED_PIN, 3, 200, 200);
    // tempSensor.reportBatteryStatus();
    blinkLed(STATUS_LED_PIN, 2, 300, 100);
    rgbLed(false); // Turn off RGB LED after activity
    
    // Blink 5 times rapidly: Report completed
    blinkLed(STATUS_LED_PIN, 5, 100, 100);
    
    // goToSleep();
  } else {
    // Not in battery mode - solid LED on
    digitalWrite(STATUS_LED_PIN, HIGH);
  }

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

  tempSensor.tick(); // Handle temperature sensor logic (reporting, retries, etc.)
  //tempSensor.reportBatteryStatus(); // Report battery status if enabled

  // Check if it's time to sleep
  if (millis() - loopStartTime >= SLEEP_DELAY_MS) {
    //goToSleep();
  }
  //factoryReset(BOOT_PIN);
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

// Put device into deep sleep
void goToSleep() {
  #if !BATTERY_ENABLED
    // Sleep mode only makes sense with battery monitoring
    return;
  #endif
  
  DEBUG_PRINTLN("Configuring wake-up sources...");
  
  // Enable timer wake-up for periodic battery checks
  esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_SECONDS * 1000000ULL);
  DEBUG_PRINTF("Timer wake-up enabled (every %d seconds)\n", TIMER_WAKEUP_SECONDS);
  
  DEBUG_PRINTLN("Entering deep sleep now...");
  DEBUG_FLUSH(); // Wait for serial to finish
  
  // Turn off LEDs to save power
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
  digitalWrite(STATUS_LED_PIN, LOW);
  // Enter deep sleep
  esp_deep_sleep_start();
}

void factoryReset(uint8_t button) {
      // Checking button for factory reset
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 10000) {
        // If key pressed for more than 10secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        // Optional set reset in factoryReset to false, to not restart device after erasing nvram, but set it to endless sleep manually instead
        Zigbee.factoryReset(false);
        Serial.println("Going to endless sleep, press RESET button or power off/on the device to wake up");
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
        esp_deep_sleep_start();
      }
    }
  }
  delay(100);
}