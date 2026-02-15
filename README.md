# ESP32-C6 Zigbee Contact Switch

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Hardware](https://img.shields.io/badge/Hardware-ESP32--C6-blue)](https://www.espressif.com/en/products/socs/esp32-c6)
[![Protocol](https://img.shields.io/badge/Protocol-Zigbee-green)](https://www.zigbee.org/)

A battery-powered Zigbee contact switch for ESP32-C6 that uses deep sleep to conserve power. Designed for use as a smart switch to control other devices. Stays awake when button is pressed.

## Features

- **Deep Sleep Mode** - Low power consumption on battery
- **Wake on Contact** - Wake up when switch state changes
- **Battery Monitoring** - Reports battery voltage via Zigbee
- **Zigbee Integration** - Works with Home Assistant ZHA
- **Periodic Check-ins** - Wakes periodically to report battery status

## Hardware

- **Microcontroller**: ESP32-C6 board (Super Mini ESP32-C6, DevKit, XIAO, or similar)
- **Contact Switch**: Push button, reed switch, magnetic contact sensor, etc
- **Power Supply**: 3.7V Li-ion battery
- **Voltage Divider Resistors**: For battery voltage monitoring (e.g., 56kΩ and 150kΩ)
- **USB-C Cable**: For programming and debugging

### Wiring

```
ESP32-C6 Pin Map:
- GPIO3: Contact switch input (with internal pull-up)
- GPIO2: Battery voltage divider (optional, for battery monitoring)
- Power: 3.7V Li-ion battery or USB-C
```

For battery monitoring, connect a voltage divider from battery positive → R1 (56kΩ) → GPIO2 → R2 (150kΩ) → GND.

## Quick Start

1. **Hardware Setup**: Connect contact switch to GPIO3 (see wiring above)
2. **Configure**: Edit [include/config.h](include/config.h) for your setup
3. **Build & Flash**: 
   - PlatformIO: `pio run -t upload -e dev`
   - For production: `pio run -t upload`
4. **Pair Device**: Put Zigbee coordinator in pairing mode, device will auto-join
5. **Test**: Open/close contact switch, device should wake and report state

## Configuration

### Key Settings in [config.h](include/config.h)

```cpp
// Pin Configuration
#define CONTACT_SWITCH_PIN 3        // Contact switch input pin
#define BATTERY_VOLTAGE_PIN 2       // Battery voltage measurement pin
#define STATUS_LED_PIN 15           // Status LED pin

// Battery Monitoring
#define BATTERY_ENABLED true        // Enable battery monitoring
#define V_DIVIDER_R1 56             // Voltage divider R1 (kΩ)
#define V_DIVIDER_R2 150            // Voltage divider R2 (kΩ)
#define BATTERY_CHECK_INTERVAL 60000 // Check battery every 60 seconds

// Sleep Configuration
#define SLEEP_DELAY_MS 5000         // Enter sleep after 5 seconds
#define TIMER_WAKEUP_SECONDS 12000  // Wake every 12000s for battery check

// Debug
#define ENABLE_SERIAL false         // Disable serial for production
```

### Partition Table

ESP32 Zigbee stack requires specific flash partitions. The project includes partition tables:

- **`partitions_zigbee.csv`** - Default partition table with OTA support
- **`partitions_zigbee_simple.csv`** - Simple partition table without OTA

The partition table is automatically configured in `platformio.ini`. See [Partition Table Documentation](docs/PARTITIONS.md) for details.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

This project was initially forked from a water flow sensor project to obtain PlatformIO ESP32-C6 configuration files. While the code has been extensively modified for a different purpose (contact switch with deep sleep vs. always-on flow monitoring), credit goes to the original project for the build system setup.

- [water-flow-zigbee by mmornati](https://github.com/mmornati/water-flow-zigbee)
- [Espressif](https://www.espressif.com/) for ESP32-C6 and Zigbee SDK
- [PIOarduino](https://github.com/pioarduino/platform-espressif32) for ESP32-C6 Arduino support
- [Home Assistant](https://www.home-assistant.io/) community

## References

- [ESP32-C6 Documentation](https://www.espressif.com/en/products/socs/esp32-c6)
- [ESP32 Zigbee SDK Documentation](https://docs.espressif.com/projects/esp-zigbee-sdk/)
- [arduino-esp repo](https://github.com/espressif/arduino-esp32)
- [Home Assistant ZHA Integration](https://www.home-assistant.io/integrations/zha/)

---

**Made for the Home Assistant and Zigbee community**
