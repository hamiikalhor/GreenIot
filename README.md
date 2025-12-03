# GreenIoT - Vertical Farming Sensor System

A simple environmental monitoring system for growing basil in vertical farms. It tracks temperature and humidity, connects wirelessly using BLE Mesh, and runs on battery for years.

## What is this?

GreenIoT is a sensor node that monitors the environment in vertical farming setups. It's designed specifically for basil cultivation, but you can adapt it for other plants too.

**Key features:**
- Measures temperature (±0.3°C) and humidity (±2% RH)
- Connects to other sensors via BLE Mesh
- Battery powered - lasts 5-7 years
- Easy to swap sensors if needed
- Production ready with full tests

## Quick Start

Get up and running in a few minutes:

```bash
# Clone the repo
git clone <repository-url>
cd GreenIot

# Go to the firmware folder
cd 03_Firmware/GreenIot

# Build it (you'll need PlatformIO installed)
pio run -e esp32-c3-devkitm-1

# Upload to your ESP32-C3 board
pio run --target upload

# Watch the output
pio device monitor
```

**What you need:**
- PlatformIO (VS Code extension or CLI)
- ESP32-C3 DevKit M-1 board
- SHT31 temperature/humidity sensor
- USB-C cable

When it boots, you should see something like:
```
========================================
  GreenIoT Vertical Farming Node
  Basil Environmental Monitoring
  Hardware: ESP32-C3
  Firmware Version: 1.0.0
========================================
[MAIN] First boot or power-on reset
[STATE_MACHINE] System initialized successfully
[I2C] Found 1 device(s): 0x44 (SHT31)
[SENSOR] Temperature: 23.45°C, Humidity: 65.3% RH
✅ Temperature OPTIMAL - Ideal for basil
✅ Humidity OPTIMAL - Ideal for basil
```

## Project Structure

```
GreenIot/
├── 01_Requirements/          # What the system needs to do
├── 02_Hardware/              # PCB designs and schematics
├── 03_Firmware/GreenIot/     # The actual code (main folder)
│   └── src/                  # Source code
├── 04_Mechanical/            # 3D models and enclosures
├── 05_Test_and_Validation/   # Test plans and results
├── 06_Production/            # BOM and manufacturing files
├── 07_Documentation/         # User manuals and guides
└── 08_Project_Management/    # Project planning stuff
```

The main code is in `03_Firmware/GreenIot/`. That's where you'll spend most of your time.

## How It Works

### Hardware

- **ESP32-C3** - The brain (RISC-V, 160MHz, 320KB RAM, 4MB Flash)
- **SHT31 sensor** - Measures temperature and humidity
- **18650 battery** - Powers everything for 5-7 years
- **BLE Mesh** - Connects multiple sensors together

**Pin connections:**
- GPIO8 → SDA (I2C)
- GPIO9 → SCL (I2C)
- GPIO10 → Sensor power control
- ADC1_CH0 → Battery voltage monitoring

### Software

The firmware is organized in layers:

1. **Core** - Entry point, handles wake-up
2. **Application** - State machine (INIT → IDLE → MEASURE → TRANSMIT → SLEEP)
3. **Services** - Power management and BLE Mesh
4. **HAL** - Hardware abstraction (easy to swap sensors)
5. **Drivers** - Low-level I2C, ADC, GPIO

**Tech stack:**
- C++17
- Arduino framework (ESP-IDF compatible)
- FreeRTOS
- PlatformIO for building

## Testing

All tests pass (28/28) with 92% code coverage.

Run tests:
```bash
cd 03_Firmware/GreenIot
./test/RUN_TESTS.sh
```

## Current Status

- ✅ **Firmware** - Production ready, v1.0.0
- ✅ **Requirements** - Complete
- ✅ **Tests** - 28/28 passing
- ⏳ **Hardware** - Design complete, validation pending
- ⏳ **Production** - BOM ready, manufacturing planned

## Documentation

- [Complete Documentation Index](DOCUMENTATION_INDEX.md) - Find everything
- [Firmware README](03_Firmware/GreenIot/README.md) - Code details
- [Requirements](01_Requirements/README.md) - System specs
- [Hardware Design](02_Hardware/README.md) - PCB info
- [Test Documentation](05_Test_and_Validation/README.md) - Testing details

## Contributing

We welcome contributions! Here's how to get started:

1. **Find or create an issue** - What do you want to work on?
2. **Create a branch** - Name it like `7-add-feature-name`
3. **Make your changes** - Write code, add tests
4. **Test everything** - Make sure all 28 tests pass
5. **Submit a pull request** - We'll review it together

**Commit messages:** Use clear, descriptive messages. Example:
```
feat(sensor): add support for SHT40 sensor

- Implement SHT40Sensor class
- Add I2C communication
- Update sensor factory

Closes #123
```

For more details, check the [Contributing Guidelines](#contributing) section below.

## Hardware Cost

Per sensor node: ~₹933,500 IRR (~$18.67 USD)

- ESP32-C3 DevKit: ₹250,000
- SHT31 Sensor: ₹180,000
- 18650 Battery: ₹150,000
- PCB & Enclosure: ₹180,000
- Other components: ₹173,500

See `06_Production/BOM/` for the complete bill of materials.

## Useful Links

- [ESP32-C3 Manual](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)
- [BLE Mesh Spec](https://www.bluetooth.com/specifications/specs/core-specification/)
- [SHT31 Datasheet](https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf)
- [PlatformIO Docs](https://docs.platformio.org/)

## License

MIT License - see LICENSE file for details.

---

**Version:** 1.0.0  
**Status:** Production Ready  
**Last Updated:** November 4, 2025

Made by the GreenIoT Team 🌱
