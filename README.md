# ESP32C6 with Zigbee on Platformio

[![CI](https://github.com/YOUR_USERNAME/water-flow-zigbee/workflows/CI/badge.svg)](https://github.com/YOUR_USERNAME/water-flow-zigbee/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Hardware](https://img.shields.io/badge/Hardware-XIAO%20ESP32C6-blue)](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)
[![Protocol](https://img.shields.io/badge/Protocol-Zigbee-green)](https://www.zigbee.org/)

Forked from water flow sensor to get the PIOarduino ini settings for a C6 board.

## 🎯 Features

- ✅ **Always-On Operation** - Zero missed pulses, accurate measurement
- ✅ **Zigbee Integration** - Works with Zigbee2MQTT and ZHA
- ✅ **Home Assistant Compatible** - Automatic device discovery
- ✅ **Optional Battery Backup** - UPS functionality with battery monitoring

## 📋 Table of Contents

- [Hardware Requirements](#hardware-requirements)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [Project Structure](#project-structure)
- [Configuration](#configuration)
- [Testing](#testing)
- [Home Assistant Setup](#home-assistant-setup)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## 🔧 Hardware Requirements

### Required Components
- **Microcontroller**: [Seeed Studio XIAO ESP32C6](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)
- **Power Supply**: 5V USB-C or external 5V supply (AC-powered)

### Optional Components
- **Battery**: 3.7V Li-ion rechargeable battery (minimum 500mAh) for UPS/backup
- **Resistors**: Two 200kΩ resistors for battery voltage divider

### Wiring Diagram

```
----
```

## 🚀 Quick Start

1. **Hardware Setup**: ~~Connect flow sensor to XIAO ESP32C6~~ (see [Installation Guide](docs/INSTALLATION.md))
2. **IDE Setup**: 
   - **PlatformIO** (recommended): See [PlatformIO Setup](docs/PLATFORMIO.md)
   - **Arduino IDE**: See [Installation Guide](docs/INSTALLATION.md)
3. **Flash Firmware**: 
   - **PlatformIO**: `pio run -t upload`, `pio run -t upload -e dev; if ($?) { pio device monitor }`
   - **Arduino IDE**: Upload `src/main.cpp` (convert to .ino if needed)
6. **Setup Zigbee**: Pair with your Zigbee coordinator
7. **Integrate Home Assistant**: Follow [Home Assistant Setup](docs/HOME_ASSISTANT.md)

## 📚 Documentation

### Installation and Setup

- **[Installation Guide](docs/INSTALLATION.md)** - Complete hardware and software installation
- **[PlatformIO Setup](docs/PLATFORMIO.md)** - PlatformIO configuration and build
- **[Home Assistant Integration](docs/HOME_ASSISTANT.md)** - Home Assistant setup and configuration

### Testing and Troubleshooting

- **[Testing Guide](docs/TESTING.md)** - Comprehensive testing procedures
- **[Troubleshooting Guide](docs/TROUBLESHOOTING.md)** - Common issues and solutions
- **[Build Troubleshooting](docs/BUILD_TROUBLESHOOTING.md)** - Build errors and solutions

### Development

- **[Contributing Guide](CONTRIBUTING.md)** - Development guidelines and contribution process

## 📁 Project Structure

```
water-flow-zigbee/
├── README.md                      # This file
├── LICENSE                         # MIT License
├── CONTRIBUTING.md                 # Contribution guidelines
├── platformio.ini                  # PlatformIO configuration
├── partitions_zigbee.csv           # Partition table (with OTA)
├── partitions_zigbee_simple.csv    # Partition table (simple, no OTA)
├── .gitignore                      # Git ignore rules
├── src/                            # Source code
│   └── main.cpp                    # Main application
├── include/                        # Header files
│   └── config.h                    # Configuration constants
├── lib/                            # Custom libraries (optional)
├── test/                           # Test files (optional)
├── examples/                       # Example code
│   ├── flow_sensor_test/           # Flow sensor test sketch
│   ├── battery_monitor_test/        # Battery monitor test sketch
│   └── calibration_test/           # Calibration test sketch
├── config/                         # Configuration files
│   └── zigbee2mqtt.yaml.example    # Zigbee2MQTT configuration example
└── docs/                           # Documentation
    ├── INSTALLATION.md             # Installation guide
    ├── PLATFORMIO.md               # PlatformIO setup guide
    ├── PARTITIONS.md               # Partition table documentation
    ├── HOME_ASSISTANT.md           # Home Assistant setup
    ├── TESTING.md                  # Testing guide
    └── TROUBLESHOOTING.md          # Troubleshooting guide
```

## ⚙️ Configuration

### Partition Table (Zigbee Required)

**Important:** ESP32 Zigbee stack requires specific flash partitions. The project includes partition tables:

- **`partitions_zigbee.csv`** - Default partition table with OTA support
- **`partitions_zigbee_simple.csv`** - Simple partition table without OTA

The partition table is automatically configured in `platformio.ini`. See [Partition Table Documentation](docs/PARTITIONS.md) for details.

### Pin Configuration
```cpp
// Battery Monitor (Optional)
#define BATTERY_PIN A0           // GPIO4 (A0)
```

### Zigbee Configuration
```cpp
// Zigbee network settings
#define ZIGBEE_CHANNEL 11         // Zigbee channel (11-26)
#define ZIGBEE_PAN_ID 0x1A62     // Your network PAN ID
```

**Important:** The Zigbee implementation in `src/main.cpp` is a template. You need to update the Zigbee functions with your ESP32 Zigbee SDK API calls.

See `include/config.h` for all configuration options.

## 🧪 Testing

### Test Sketches

The `examples/` directory contains test sketches:

2. **battery_monitor_test.ino** - Battery monitoring testing (if enabled)

### Testing Procedure

3. Test complete system: Upload main sketch and verify all functionality

See [Testing Guide](docs/TESTING.md) for comprehensive testing procedures.

## 🏠 Home Assistant Setup

The device automatically exposes these entities in Home Assistant:


See [Home Assistant Integration](docs/HOME_ASSISTANT.md) for detailed setup instructions.

## 🎓 How It Works

### Always-On Architecture

This project uses **always-on operation** (no sleep modes) to ensure zero missed pulses:

6. **Zigbee Reporting** - Reports to coordinator every 30 seconds

### Why Always-On?

Deep sleep causes **missed pulses** during sleep/wake transitions:
- Interrupts are disabled during deep sleep
- Wake-up latency causes pulse loss
- Boot time delays interrupt initialization

With AC power, always-on operation provides:
- ✅ Zero missed pulses
- ✅ Real-time measurements
- ✅ Simplified code
- ✅ Better reliability

## 🔍 Troubleshooting

Common issues and solutions are documented in:

- **[TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md)** - Detailed troubleshooting guide

### Common Issues

**Zigbee won't join network**
- Reset Zigbee network: hold BOOT button, press RESET
- Check Zigbee channel matches coordinator
- Ensure coordinator is in pairing mode

See [Troubleshooting Guide](docs/TROUBLESHOOTING.md) for more solutions.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [Seeed Studio](https://www.seeedstudio.com/) for XIAO ESP32C6
- [Espressif](https://www.espressif.com/) for ESP32 Zigbee SDK
- [Home Assistant](https://www.home-assistant.io/) community

## 📚 References

- [XIAO ESP32C6 Wiki](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)
- [ESP32 Zigbee SDK Documentation](https://docs.espressif.com/projects/esp-zigbee-sdk/)
- [Zigbee2MQTT Documentation](https://www.zigbee2mqtt.io/)
- [Home Assistant ZHA Integration](https://www.home-assistant.io/integrations/zha/)

---

**Made with ❤️ for the Home Assistant community**
