# Partition Table Configuration

This document explains the partition table configuration required for Zigbee devices on ESP32.

## Why Partition Tables Are Required

ESP32 Zigbee stack requires specific flash memory partitions for:
- **Zigbee NVRAM Storage** (`zb_storage`) - Stores Zigbee network data, device information, and network keys
- **Zigbee Factory Partition** (`zb_fct`) - Stores factory calibration and configuration data

Without these partitions, the Zigbee stack will fail to initialize.

## Available Partition Tables

This project includes two partition table options:

### 1. `partitions_zigbee.csv` (Default - With OTA Support)

**Use this if:** You want over-the-air (OTA) update capability.

**Partitions:**
- `nvs` (20KB) - Non-volatile storage for Zigbee network data
- `otadata` (8KB) - OTA update metadata
- `app0` (1.25MB) - Primary application partition
- `app1` (1.25MB) - Secondary application partition (for OTA updates)
- `spiffs` (1.34MB) - SPIFFS filesystem for EEPROM/preferences data
- `zb_storage` (16KB) - Zigbee NVRAM storage (required)
- `zb_fct` (4KB) - Zigbee factory partition (required)
- `coredump` (64KB) - Crash dump storage

**Total:** 4MB (typical ESP32-C6 flash size)

### 2. `partitions_zigbee_simple.csv` (Simple - No OTA)

**Use this if:** You don't need OTA updates and want more space for the application.

**Partitions:**
- `nvs` (20KB) - Non-volatile storage for Zigbee network data
- `app` (2.94MB) - Single application partition (no OTA)
- `spiffs` (1.0MB) - SPIFFS filesystem for EEPROM/preferences data
- `zb_storage` (16KB) - Zigbee NVRAM storage (required)
- `zb_fct` (4KB) - Zigbee factory partition (required)
- `coredump` (64KB) - Crash dump storage

**Total:** 4MB (typical ESP32-C6 flash size)

## Configuration

### Select Partition Table

Edit `platformio.ini`:

```ini
[env:esp32c6]
; For OTA support (default)
board_build.partitions = partitions_zigbee.csv

; OR for simple (no OTA)
board_build.partitions = partitions_zigbee_simple.csv
```

### Build with Selected Partition Table

```bash
# Build with default partition table
pio run

# Verify build
pio run -v
```

## Partition Details

### Required Zigbee Partitions

**zb_storage (16KB)**
- Type: `data`
- SubType: `fat`
- Purpose: Stores Zigbee network configuration, device keys, and network data
- **Critical:** Without this, Zigbee stack will not initialize

**zb_fct (4KB)**
- Type: `data`
- SubType: `fat`
- Purpose: Factory calibration and Zigbee device configuration
- **Critical:** Required for Zigbee stack initialization

### Optional Partitions

**nvs (20KB)**
- Stores system and application NVS data
- Used by Preferences library (EEPROM functionality)

**spiffs (1-1.34MB)**
- SPIFFS filesystem
- Can be used for additional data storage
- Used by Preferences library

**app0/app1 (1.25MB each with OTA)**
- Application firmware partitions
- `app0` is active, `app1` used for OTA updates

**coredump (64KB)**
- Crash dump storage
- Useful for debugging crashes

## Creating Custom Partition Tables

If you need a custom partition layout:

1. **Calculate Total Flash:**
   - Most ESP32-C6 boards have 4MB = 0x400000 bytes
   - Total partition size must not exceed this

2. **Required Partitions:**
   - Keep `zb_storage` (16KB minimum)
   - Keep `zb_fct` (4KB minimum)
   - Keep `nvs` (recommended, 20KB)
   - Keep application partition

3. **Create CSV File:**
   ```csv
   # Name,     Type, SubType, Offset,  Size,     Flags
   nvs,        data, nvs,     0x9000,  0x5000,
   app,        app,  factory, 0x10000, 0x2E0000,
   zb_storage, data, fat,     0x2F0000,0x4000,
   zb_fct,     data, fat,     0x2F4000,0x1000,
   ```

4. **Verify Offsets:**
   - First partition starts at 0x9000 (bootloader uses 0x0-0x8FFF)
   - Partitions must not overlap
   - Total size must fit in 4MB

## Troubleshooting

### Issue: Zigbee stack won't initialize

**Possible Causes:**
- Partition table missing `zb_storage` or `zb_fct`
- Partition table not referenced in `platformio.ini`
- Partition sizes too small

**Solutions:**
1. Verify partition table includes `zb_storage` and `zb_fct`
2. Check `platformio.ini` has `board_build.partitions = partitions_zigbee.csv`
3. Verify partition sizes (zb_storage: 16KB minimum, zb_fct: 4KB minimum)

### Issue: Build fails with partition error

**Possible Causes:**
- Partition table syntax error
- Partitions exceed flash size
- Invalid partition types/subtypes

**Solutions:**
1. Verify CSV format (comma-separated, no spaces in names)
2. Check total partition size ≤ 4MB
3. Verify partition types are valid (data, app)
4. Check SubTypes are valid (nvs, fat, ota_0, ota_1, etc.)

### Issue: OTA not working

**Possible Causes:**
- Using simple partition table (no OTA support)
- Partition table missing `app1` partition
- `otadata` partition missing

**Solutions:**
1. Use `partitions_zigbee.csv` (includes OTA partitions)
2. Verify `app0` and `app1` partitions exist
3. Verify `otadata` partition exists

## References

- [ESP32 Partition Tables](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)
- [PlatformIO Partition Tables](https://docs.platformio.org/en/latest/platforms/espressif32.html#partition-tables)
- [ESP32 Zigbee SDK Documentation](https://docs.espressif.com/projects/esp-zigbee-sdk/)


