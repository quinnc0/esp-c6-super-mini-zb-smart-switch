/*
 * Water Flow Meter - Configuration
 * All adjustable parameters for the water flow meter project
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Pin Configuration
// ============================================================================

#define FLOW_SENSOR_PIN 2       // GPIO2 (D2) - YF-S201 flow sensor signal
#define BATTERY_PIN 0           // GPIO0 - Battery voltage monitor (optional)
#define LED_PIN 8    // Built-in LED for status indication

// ============================================================================
// Flow Sensor Configuration
// ============================================================================

// Calibration factor: pulses per liter
// Standard YF-S201: 7.5 pulses/L (1 pulse ≈ 2.25ml)
// Adjust based on calibration testing
#define CALIBRATION_FACTOR 7.5

// Flow calculation interval (milliseconds)
#define FLOW_CALC_INTERVAL 1000    // Calculate flow every 1 second

// Flow idle timeout (milliseconds)
#define FLOW_IDLE_TIMEOUT 5000     // Consider idle if no pulses for 5 seconds

// ============================================================================
// Battery Configuration (Optional)
// ============================================================================

// Enable battery monitoring (set to true if battery backup installed)
#define BATTERY_ENABLED false

// Battery voltage limits
#define BATTERY_MIN_VOLTAGE 3.0    // Minimum voltage (empty battery)
#define BATTERY_MAX_VOLTAGE 4.2    // Maximum voltage (full battery)

// Battery monitoring interval (milliseconds)
#define BATTERY_CHECK_INTERVAL 60000  // Check battery every minute

// Battery warning levels
#define BATTERY_WARNING_LEVEL 25      // Warning at 25%
#define BATTERY_CRITICAL_LEVEL 10     // Critical at 10%

// ============================================================================
// Zigbee Configuration
// ============================================================================

// Zigbee network settings
#define ZIGBEE_CHANNEL 15         // Zigbee channel (11-26, avoid WiFi channels)
#define ZIGBEE_PAN_ID 0xA209     // Personal Area Network ID (use your coordinator's PAN ID)

// Device endpoints
#define FLOW_ENDPOINT 10         // Flow measurement endpoint
#define BATTERY_ENDPOINT 1       // Battery endpoint (optional)

// Zigbee report intervals (seconds)
#define FLOW_REPORT_INTERVAL 3      // Report flow every 3 seconds
#define BATTERY_REPORT_INTERVAL 600  // Report battery every 10 minutes

// Report triggers
#define FLOW_RATE_CHANGE_THRESHOLD 0.1  // Report if flow rate changes by >10%
#define VOLUME_MILESTONE 1.0             // Report every 1 liter
#define BATTERY_CHANGE_THRESHOLD 5       // Report if battery changes by >5%

// ============================================================================
// Serial Configuration
// ============================================================================

#define SERIAL_BAUD_RATE 115200      // Serial baud rate for debugging
#ifndef DEBUG_ENABLED
#define DEBUG_ENABLED true           // Enable debug serial output
#endif

// ============================================================================
// System Configuration
// ============================================================================

// System status LED blink interval (milliseconds)
#define STATUS_LED_INTERVAL 1000    // Blink LED every second when idle

// Watchdog timeout (if implemented)
// #define WATCHDOG_TIMEOUT 60000   // 60 seconds (optional)

#endif // CONFIG_H

