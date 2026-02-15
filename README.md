# ESP32-C6 Zigbee Contact Switch

[![CI](https://github.com/YOUR_USERNAME/zigbee-contact-switch/workflows/CI/badge.svg)](https://github.com/YOUR_USERNAME/zigbee-contact-switch/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Hardware](https://img.shields.io/badge/Hardware-ESP32--C6-blue)](https://www.espressif.com/en/products/socs/esp32-c6)
[![Protocol](https://img.shields.io/badge/Protocol-Zigbee-green)](https://www.zigbee.org/)

A battery-powered Zigbee contact switch for ESP32-C6 that uses deep sleep to conserve power. Perfect for door/window sensors, mailbox notifications, or any open/closed monitoring application.

## 🎯 Features

- ✅ **Deep Sleep Mode** - Ultra-low power consumption on battery
- ✅ **Wake on Contact** - Instant wake up when switch state changes
- ✅ **Battery Monitoring** - Reports battery voltage via Zigbee
- ✅ **Zigbee Integration** - Works with Zigbee2MQTT and Home Assistant ZHA
- ✅ **IAS Zone Support** - Uses standard Zigbee IAS (Intruder Alarm System) Zone cluster
- ✅ **Periodic Check-ins** - Wakes periodically to report battery status

## 📋 Table of Contents

- [Hardware Requirements](#hardware-requirements)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [Project Structure](#project-structure)
- [Configuration](#configuration)
- [Testing](#testing)
- [Home Assistant Setup](#home-assistant-setup)
- [How It Works](#how-it-works)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## 🔧 Hardware Requirements

### Required Components
- **Microcontroller**: ESP32-C6 board (Super Mini ESP32-C6, DevKit, or similar)
- **Contact Switch**: Reed switch, magnetic contact sensor, or button
- **Power Supply**: 3.7V Li-ion battery (recommended 500mAh or larger)

### Optional Components
- **Voltage Divider Resistors**: For battery voltage monitoring (e.g., 56kΩ and 150kΩ)
- **USB-C Cable**: For programming and debugging

### Wiring

```
ESP32-C6 Pin Map:
- GPIO3: Contact switch input (with internal pull-up)
- GPIO2: Battery voltage divider (optional, for battery monitoring)
- GPIO15: Status LED (optional)
- Power: 3.7V Li-ion battery or USB-C
```

For battery monitoring, connect a voltage divider from battery positive → R1 (56kΩ) → GPIO2 → R2 (150kΩ) → GND.

## 🚀 Quick Start

1. **Hardware Setup**: Connect contact switch to GPIO3 (see wiring above)
2. **Configure**: Edit [include/config.h](include/config.h) for your setup
3. **Build & Flash**: 
   - PlatformIO: `pio run -t upload -e dev`
   - For production: `pio run -t upload`
4. **Pair Device**: Put Zigbee coordinator in pairing mode, device will auto-join
5. **Test**: Open/close contact switch, device should wake and report state

## 📚 Documentation

### Installation and Setup

- **[Installation Guide](docs/INSTALLATION.md)** - Complete hardware and software installation
- **[PlatformIO Setup](docs/PLATFORMIO.md)** - PlatformIO configuration and build
- **[Home Assistant Integration](docs/HOME_ASSISTANT.md)** - Home Assistant setup and configuration

### Testing and Troubleshooting

- **[Testing Guide](docs/TESTING.md)** - Comprehensive testing procedures
- **[Testing Suite](docs/TESTING_SUITE.md)** - Automated test suite documentation
- **[Troubleshooting Guide](docs/TROUBLESHOOTING.md)** - Common issues and solutions
- **[Build Troubleshooting](docs/BUILD_TROUBLESHOOTING.md)** - Build errors and solutions

### Development

- **[Contributing Guide](CONTRIBUTING.md)** - Development guidelines and contribution process
- **[Partition Tables](docs/PARTITIONS.md)** - Flash partition configuration

## 📁 Project Structure

```
zigbee-contact-switch/
├── README.md                      # This file
├── LICENSE                        # MIT License
├── CONTRIBUTING.md                # Contribution guidelines
├── platformio.ini                 # PlatformIO configuration
├── partitions_zigbee.csv          # Partition table (with OTA)
├── partitions_zigbee_simple.csv   # Partition table (simple, no OTA)
├── src/
│   ├── main.cpp                   # Main application
│   └── contactSwitch.cpp          # Contact switch implementation
├── include/
│   ├── config.h                   # Configuration constants
│   ├── contactSwitch.h            # Contact switch header
│   └── sensor.h                   # Sensor utilities
├── test/                          # Unit tests
│   ├── test_main.cpp
│   ├── test_battery_monitor.cpp
│   └── [...other tests]
├── examples/                      # Example sketches
│   ├── flow_sensor_test/
│   ├── battery_monitor_test/
│   └── calibration_test/
├── config/
│   └── zigbee2mqtt.yaml.example   # Zigbee2MQTT configuration example
└── docs/                          # Documentation
    ├── INSTALLATION.md
    ├── PLATFORMIO.md
    ├── HOME_ASSISTANT.md
    └── [...other docs]
```

## ⚙️ Configuration

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

### Partition Table (Zigbee Required)

**Important:** ESP32 Zigbee stack requires specific flash partitions. The project includes partition tables:

- **`partitions_zigbee.csv`** - Default partition table with OTA support
- **`partitions_zigbee_simple.csv`** - Simple partition table without OTA

The partition table is automatically configured in `platformio.ini`. See [Partition Table Documentation](docs/PARTITIONS.md) for details.

### Build Environments

Three build environments are available in `platformio.ini`:

- **`esp32c6`** (default) - Production build with minimal debug output
- **`dev`** - Development build with debug output and serial logging
- **`test`** - Test environment for running unit tests

```bash
# Production build
pio run -t upload

# Development build with monitoring
pio run -t upload -e dev
pio device monitor

# Run tests
pio test -e test
```

## 🧪 Testing

### Hardware Testing

1. **Contact Switch Test**: Open/close the contact switch and verify the device wakes and reports state
2. **Battery Test**: Monitor battery voltage reporting in Home Assistant
3. **Sleep Test**: Verify device enters deep sleep after configured delay
4. **Wake Test**: Confirm device wakes on contact switch activation

### Unit Tests

The project includes comprehensive unit tests in the `test/` directory:

```bash
# Run all tests
pio test -e test

# Run specific test
pio test -e test -f test_battery_monitor
```

See [Testing Guide](docs/TESTING.md) and [Testing Suite](docs/TESTING_SUITE.md) for comprehensive testing procedures.

## 🏠 Home Assistant Setup

The device automatically appears in Home Assistant when using Zigbee2MQTT or ZHA:

### Exposed Entities

- **Binary Sensor** (Contact): Reports contact state (open/closed)
- **Sensor** (Battery): Battery voltage in volts
- **Sensor** (Battery Percentage): Estimated battery percentage

### Zigbee2MQTT

The device will be auto-discovered. You can customize the configuration in Zigbee2MQTT:

```yaml
# Example friendly name and icon configuration
'0x00124b001234abcd':
  friendly_name: 'Front Door Sensor'
  icon: 'mdi:door'
```

See [Home Assistant Integration](docs/HOME_ASSISTANT.md) for detailed setup instructions.

## 🎓 How It Works

### Deep Sleep Architecture

This project uses **deep sleep mode** to minimize power consumption for battery operation:

1. **Active Period**: Device wakes up, connects to Zigbee network, reports status
2. **Contact Monitor**: Monitors contact switch state with internal pull-up
3. **Sleep Decision**: After configurable delay (5s default), enters deep sleep
4. **Wake Sources**:
   - **Contact Change**: GPIO wake-up when contact switch changes state (instant)
   - **Timer**: Periodic wake-up every ~3.3 hours to report battery status

### Power Consumption

- **Deep Sleep**: ~10-50 µA (Ultra-low power)
- **Active (Zigbee connected)**: ~20-50 mA
- **Wake-up Duration**: ~1-3 seconds per wake cycle

**Battery Life Estimate** (with 1000mAh battery):
- Infrequent activations (1-2 per day): **6-12 months**
- Moderate use (10-20 per day): **3-6 months**  
- High use (50+ per day): **1-2 months**

### Why Deep Sleep?

For battery-powered contact sensors, deep sleep is essential:
- ✅ Extended battery life (months to years)
- ✅ Instant wake on contact change
- ✅ Maintains Zigbee network membership
- ✅ Periodic battery status reporting

The trade-off is brief wake-up latency (~1-2 seconds), which is acceptable for most contact sensor applications.

## 🔍 Troubleshooting

### Common Issues

**Device won't wake from sleep**
- Check that `BATTERY_ENABLED` is set to `true` in config.h
- Verify contact switch is properly connected to GPIO3
- Ensure internal pull-up is enabled (handled in code)

**Battery voltage not reporting**
- Verify voltage divider is correctly connected
- Check resistor values match config.h settings
- Enable serial debug and check voltage readings

**Zigbee won't join network**
- Hold contact switch closed during pairing
- Check Zigbee coordinator is in pairing mode
- Try power cycling the device

**Device resets unexpectedly**
- Battery voltage may be too low (< 3.0V)
- Check power supply connections
- Disable sleep mode for debugging

See [Troubleshooting Guide](docs/TROUBLESHOOTING.md) for more solutions.

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on:

- Code style and formatting
- Submitting pull requests
- Reporting issues
- Development setup

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

This project was initially forked from a water flow sensor project to obtain PlatformIO ESP32-C6 configuration files. While the code has been extensively modified for a different purpose (contact switch with deep sleep vs. always-on flow monitoring), credit goes to the original project for the build system setup.

- [Espressif](https://www.espressif.com/) for ESP32-C6 and Zigbee SDK
- [PIOarduino](https://github.com/pioarduino/platform-espressif32) for ESP32-C6 Arduino support
- [Home Assistant](https://www.home-assistant.io/) community
- Original water flow sensor project for PlatformIO configuration reference

## 📚 References

- [ESP32-C6 Documentation](https://www.espressif.com/en/products/socs/esp32-c6)
- [ESP32 Zigbee SDK Documentation](https://docs.espressif.com/projects/esp-zigbee-sdk/)
- [Zigbee2MQTT Documentation](https://www.zigbee2mqtt.io/)
- [Home Assistant ZHA Integration](https://www.home-assistant.io/integrations/zha/)
- [PlatformIO Documentation](https://docs.platformio.org/)

---

**Made for the Home Assistant and Zigbee community** 🏠⚡
