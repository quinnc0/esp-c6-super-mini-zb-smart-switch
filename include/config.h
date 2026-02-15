#pragma once

// ============================================================================
// Serial Debug Configuration
// ============================================================================

// Set to false for production to save power and boot time
#define ENABLE_SERIAL false

#if ENABLE_SERIAL
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, __VA_ARGS__)
  #define DEBUG_FLUSH() Serial.flush()
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(fmt, ...)
  #define DEBUG_FLUSH()
#endif

// ============================================================================
// Pin Configuration
// ============================================================================

#define CONTACT_SWITCH_PIN 3
#define STATUS_LED_PIN 15
#define BATTERY_VOLTAGE_PIN 2

// ============================================================================
// Battery Configuration (Optional)
// ============================================================================

// Enable battery monitoring (set to true if battery backup installed)
#define BATTERY_ENABLED true

// Voltage divider resistors for battery voltage measurement (in kΩ)
// Adjust these values based on your actual resistor values
#define V_DIVIDER_R1 55 //39; // Resistor between battery and pin
#define V_DIVIDER_R2 99 //100; // Resistor between pin and ground

// Battery monitoring interval (milliseconds)
#define BATTERY_CHECK_INTERVAL 60000  // Check battery every minute

// ============================================================================
// Zigbee Configuration
// ============================================================================

// Device endpoints
#define SMART_SWITCH 10

// Zigbee network settings (probably not needed)
#define ZIGBEE_CHANNEL 15         // Zigbee channel (11-26, avoid WiFi channels)
#define ZIGBEE_PAN_ID 0xA209     // Personal Area Network ID (use your coordinator's PAN ID)

// ============================================================================
// Sleep Configuration
// ============================================================================

#define SLEEP_DELAY_MS 5000         // Sleep after 5 seconds in loop
#define TIMER_WAKEUP_SECONDS 300    // Wake up every 5 minutes for battery check (only used if BATTERY_ENABLED)

// ============================================================================
// Serial Configuration
// ============================================================================

#define SERIAL_BAUD_RATE 115200      // Serial baud rate for debugging

