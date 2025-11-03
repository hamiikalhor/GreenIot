# GreenIoT Vertical Farming Sensor Node

**Environmental Monitoring System for Basil Cultivation**

![Architecture](https://img.shields.io/badge/Architecture-Layered-blue)
![Platform](https://img.shields.io/badge/Platform-ESP32--C3-green)
![Framework](https://img.shields.io/badge/Framework-Arduino-teal)
![Build](https://img.shields.io/badge/Build-PlatformIO-orange)

---

## 📋 Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Hardware Requirements](#hardware-requirements)
- [Software Structure](#software-structure)
- [Getting Started](#getting-started)
- [Sensor Abstraction Layer](#sensor-abstraction-layer)
- [Adding New Sensors](#adding-new-sensors)
- [Configuration](#configuration)
- [API Documentation](#api-documentation)
- [Testing](#testing)

---

## 🌱 Overview

This firmware implements an **environmental monitoring sensor node** for vertical farming systems, specifically optimized for **basil (Ocimum basilicum)** cultivation. The design emphasizes:

- ✅ **Hardware Abstraction** - Easy sensor replacement (I2C, SPI, UART)
- ✅ **Power Efficiency** - Battery-powered operation with BLE Mesh
- ✅ **Layered Architecture** - Clean separation of concerns
- ✅ **Extensibility** - Add sensors without modifying application code

### Key Features

- **Precision Monitoring**: Temperature (±0.3°C) and Humidity (±2% RH)
- **Sensor Agnostic**: Swap SHT31 ↔ AHT20 ↔ HDC2080 with zero code changes
- **Real-time Assessment**: Basil-specific growth condition analysis
- **BLE Mesh Ready**: Designed for multi-node mesh networks
- **Low Power**: Optimized for battery operation (target: 6+ months)

---

## 🏗️ Architecture

### Layered Design

The firmware follows a **strict layered architecture** with clean separation of concerns:

```
┌─────────────────────────────────────────────────────────┐
│              ENTRY POINT LAYER                          │
│  - main.cpp: Arduino setup()/loop() only (82 lines)    │
│  - Minimal bootstrapping, delegates to System class     │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              APPLICATION LAYER ⭐                       │
│  - system.h/cpp: System Manager class                   │
│  - Orchestrates all subsystems                          │
│  - Business logic for basil monitoring                  │
│  - Measurement scheduling and data assessment           │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              SERVICE LAYER (Future)                     │
│  - data_manager: Data buffering, validation             │
│  - comm_service: BLE Mesh communication                 │
│  - power_manager: Sleep modes & power optimization      │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│      HARDWARE ABSTRACTION LAYER (HAL)                   │
│  HAL/Sensor/                                            │
│    - sensor_interface.h: Abstract sensor interface      │
│    - sht31_driver.cpp: SHT31 implementation             │
│    - aht20_driver.cpp: AHT20 implementation (future)    │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              PERIPHERAL DRIVER LAYER                    │
│  Drivers/                                               │
│    - i2c_driver.h/cpp: I2C hardware abstraction         │
│    - spi_driver.h/cpp: SPI abstraction (future)         │
│    - uart_driver.h/cpp: UART abstraction (future)       │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              HARDWARE LAYER                             │
│  - ESP32-C3 peripherals (BLE 5.0, I2C, SPI, UART)      │
│  - Arduino Wire.h, SPI.h, HardwareSerial               │
└─────────────────────────────────────────────────────────┘
```

### Key Principles

✅ **Separation of Concerns**: `main.cpp` is just an entry point, `System` class manages everything  
✅ **Single Responsibility**: Each class/module has one clear purpose  
✅ **Dependency Inversion**: Upper layers depend on abstractions, not implementations  
✅ **Hardware Abstraction**: No direct hardware access in application layer

---

## 🔌 Hardware Requirements

### Microcontroller

- **ESP32-C3 DevKit M-1**
  - 32-bit RISC-V @ 160MHz
  - 400KB SRAM, 4MB Flash
  - Built-in BLE 5.0 with Mesh support
  - Ultra-low power consumption

### Sensors (Choose One)

#### Primary: SHT31 (Sensirion)
- **Interface**: I2C (0x44 or 0x45)
- **Temperature**: -40°C to +125°C (±0.3°C accuracy)
- **Humidity**: 0-100% RH (±2% accuracy)
- **Power**: 800µA active, <0.5µA sleep
- **Available**: ✅ Iranian market (Digi-Key, Mouser distributors)

#### Alternative: AHT20 (Aosong)
- **Interface**: I2C (0x38)
- **Temperature**: -40°C to +85°C (±0.3°C accuracy)
- **Humidity**: 0-100% RH (±2% accuracy)
- **Available**: ✅ Iranian market (cheaper than SHT31)

### Connections

```
ESP32-C3          SHT31/AHT20
────────          ───────────
GPIO 8 (SDA)  →   SDA
GPIO 9 (SCL)  →   SCL
3.3V          →   VCC
GND           →   GND
```

### Bill of Materials (BOM)

| Component | Part Number | Quantity | Est. Price (Iran) | Supplier |
|-----------|-------------|----------|-------------------|----------|
| ESP32-C3 DevKit M-1 | ESP32-C3-DevKitM-1 | 1 | 250,000 IRR | Digi-Key Iran |
| SHT31 Sensor | SHT31-DIS-B | 1 | 180,000 IRR | Mouser Iran |
| Pull-up Resistors (I2C) | 4.7kΩ 1% | 2 | 2,000 IRR | Local |
| LiPo Battery | 3.7V 2000mAh | 1 | 150,000 IRR | Local |
| PCB (Custom) | - | 1 | 50,000 IRR | PCBWay Iran |
| **Total** | - | - | **~632,000 IRR** | **~$12 USD** |

---

## 📁 Software Structure

### Directory Layout

```
03_Firmware/GreenIot/
├── include/                        # Header files
│   ├── system.h                    # ⭐ System Manager (NEW)
│   ├── Drivers/                    # Peripheral driver headers
│   │   ├── i2c_driver.h           # I2C abstraction
│   │   ├── spi_driver.h           # SPI abstraction (future)
│   │   └── uart_driver.h          # UART abstraction (future)
│   └── HAL/                        # Hardware Abstraction Layer
│       └── Sensor/                 # Sensor HAL
│           └── sensor_interface.h # Abstract sensor interface
│
├── src/                            # Source files
│   ├── main.cpp                    # ⭐ Entry point (82 lines, clean!)
│   ├── system.cpp                  # ⭐ System Manager (NEW)
│   ├── Drivers/                    # Peripheral driver implementations
│   │   ├── i2c_driver.cpp         # I2C driver for ESP32
│   │   ├── spi_driver.cpp         # SPI driver (future)
│   │   └── uart_driver.cpp        # UART driver (future)
│   └── HAL/                        # HAL implementations
│       └── Sensor/                 # Sensor drivers
│           ├── sensor_interface.cpp  # Sensor registry
│           ├── sht31_driver.cpp      # SHT31 implementation
│           └── aht20_driver.cpp      # AHT20 (future)
│
├── platformio.ini                  # PlatformIO configuration
└── README.md                       # This file
```

### File Descriptions

| File | Lines | Purpose |
|------|-------|---------|
| **main.cpp** | 82 | Clean entry point - just calls `greeniot.init()` and `greeniot.run()` |
| **system.h** | 118 | System Manager interface - `init()`, `run()`, `stop()` methods |
| **system.cpp** | 342 | All application logic - initialization, measurement, assessment |
| **sensor_interface.h** | - | Abstract sensor interface (HAL) |
| **sht31_driver.cpp** | - | Concrete SHT31 sensor implementation |
| **i2c_driver.cpp** | - | I2C peripheral driver (wraps Wire.h) |

---

## 🚀 Getting Started

### Prerequisites

1. **PlatformIO** installed (VS Code extension or CLI)
2. **ESP32-C3** board drivers
3. **USB cable** for programming

### Installation

```bash
# Clone the repository
cd 03_Firmware/GreenIot

# Install dependencies (automatic with PlatformIO)
pio lib install

# Build the project
pio run

# Upload to ESP32-C3
pio run --target upload

# Open serial monitor
pio device monitor
```

### First Boot

Upon successful upload, you should see:

```
╔══════════════════════════════════════════╗
║   GreenIoT Vertical Farming Project      ║
║   Environmental Monitoring Sensor Node   ║
║   Layered Architecture Implementation    ║
╚══════════════════════════════════════════╝

Architecture Layers:
  [4] Application Layer    ← System Manager
  [3] HAL Layer            ← sensor_interface
  [2] Peripheral Driver    ← i2c_driver
  [1] Hardware Layer       ← ESP32-C3 / Wire.h

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Initializing Peripheral Driver Layer...
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

I2C initialized (SDA=8, SCL=9, 100000 Hz)
Scanning I2C bus...
Found 1 device(s):
  - 0x44

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Initializing HAL (Sensor Abstraction)...
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Sensor interface acquired: SHT31
Initializing sensor hardware...
Sensor initialized successfully
Sensor configured (HIGH precision mode)

Sensor Information:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  Model:         SHT31
  Manufacturer:  Sensirion
  Interface:     I2C
  Temp Range:    -40.0°C to 125.0°C
  Humidity Range: 0% to 100%
  Temp Accuracy:  ±0.3°C
  Hum Accuracy:   ±2.0%
  ...

System initialized successfully
```

---

## 🎯 Sensor Abstraction Layer

### The Problem We Solved

**Bad Approach** (tightly coupled):
```c
// ❌ Application directly uses SHT31 functions
void measure() {
    sht31_wakeup();           // Hardcoded to SHT31!
    sht31_trigger();
    float temp = sht31_read_temp();
}
```

**Good Approach** (abstracted):
```c
// ✅ Application uses abstract interface
void measure() {
    sensor->wakeup();         // Works with ANY sensor!
    sensor->trigger_measurement();
    sensor_data_t data;
    sensor->read(&data);
}
```

### How It Works

1. **Define Interface** (`sensor_interface.h`):
```c
typedef struct {
    sensor_status_t (*init)(void);
    sensor_status_t (*read)(sensor_data_t *data);
    // ... more functions
} sensor_interface_t;
```

2. **Implement Driver** (`sht31_driver.cpp`):
```cpp
extern "C" const sensor_interface_t sht31_driver = {
    .init = sht31_init,
    .read = sht31_read,
    // ... implementations
};
```

3. **Register Sensor** (`sensor_interface.cpp`):
```cpp
static const sensor_registry_entry_t sensor_registry[] = {
    { "SHT31", &sht31_driver },
    { "AHT20", &aht20_driver },  // Easy to add!
};
```

4. **Use in Application** (`system.cpp`):
```cpp
const sensor_interface_t *sensor = sensor_get_interface("SHT31");
sensor->init();
sensor_data_t data;
sensor->read(&data);
```

### Benefits

- ✅ **Swap sensors**: Change `"SHT31"` to `"AHT20"` - done!
- ✅ **Support any interface**: I2C, SPI, UART, analog - all use same API
- ✅ **Unit testing**: Mock sensors for testing
- ✅ **Future-proof**: Add sensors without touching application code

---

## ➕ Adding New Sensors

### Example: Adding AHT20 Sensor

**Step 1**: Create driver file `src/HAL/Sensor/aht20_driver.cpp`:

```cpp
#include "HAL/Sensor/sensor_interface.h"
#include "Drivers/i2c_driver.h"
#include "esp_log.h"

static const char *TAG = "AHT20";

#define AHT20_I2C_ADDR 0x38

// Implement all interface functions
static sensor_status_t aht20_init(void) {
    ESP_LOGI(TAG, "Initializing AHT20...");
    // AHT20-specific initialization
}

static sensor_status_t aht20_read(sensor_data_t *data) {
    // AHT20-specific read logic
}

// ... implement other functions

// Export interface (C linkage for registry)
extern "C" const sensor_interface_t aht20_driver = {
    .init = aht20_init,
    .read = aht20_read,
    // ... rest of functions
};
```

**Step 2**: Register in `src/HAL/Sensor/sensor_interface.cpp`:

```cpp
extern const sensor_interface_t aht20_driver;  // Add extern

static const sensor_registry_entry_t sensor_registry[] = {
    { "SHT31", &sht31_driver },
    { "AHT20", &aht20_driver },  // Add to registry
};
```

**Step 3**: Use in `system.cpp`:

```cpp
#define SENSOR_MODEL "AHT20"  // Change this line only!
```

**That's it!** No other code changes needed. The System class handles everything else.

### Adding UART Sensor (e.g., MH-Z19 CO2)

Same process, but driver uses `uart_driver.h` instead of `i2c_driver.h`:

```cpp
#include "Drivers/uart_driver.h"

static sensor_status_t mhz19_init(void) {
    uart_config_t config = {
        .tx_pin = 4,
        .rx_pin = 5,
        .baud_rate = 9600,
        .timeout_ms = 1000
    };
    return (uart_init(&config) == UART_OK) ? SENSOR_OK : SENSOR_ERROR_INIT;
}

extern "C" const sensor_interface_t mhz19_driver = { /* ... */ };
```

The **System class doesn't know or care** if sensor uses I2C, UART, or SPI!

---

## ⚙️ Configuration

### Sensor Selection

Change sensor in `src/system.cpp`:

```cpp
#define SENSOR_MODEL "SHT31"  // Options: "SHT31", "AHT20", etc.
```

### I2C Configuration

In `src/system.cpp`:

```cpp
#define I2C_SDA_PIN 8          // SDA pin
#define I2C_SCL_PIN 9          // SCL pin
#define I2C_FREQUENCY_HZ 100000  // 100kHz standard mode
```

### Sampling Interval

In `src/system.cpp`:

```cpp
#define SAMPLING_INTERVAL_MS 300000  // 5 minutes (300000ms)
```

**Rationale for 5-minute sampling:**
- Basil responds to temperature changes within 10-15 minutes
- 5-minute interval catches microclimate variations
- Balances data granularity vs. battery life
- Results in ~7 months battery life (2000mAh LiPo)

### Sensor Precision

In `system.cpp` `initSensor()`:

```cpp
sensor_config_t config = {
    .precision = 2,  // 0=low (4ms), 1=medium (6ms), 2=high (15ms)
    .temp_offset_celsius = 0.0f,   // Calibration offset
    .hum_offset_percent = 0.0f,    // Calibration offset
    .enable_heater = false         // Built-in heater
};
```

### Serial Baud Rate

In `src/main.cpp`:

```cpp
#define SERIAL_BAUD 115200  // Serial monitor baud rate
```

---

## 📚 API Documentation

### System Manager API

#### System Class (`system.h`)

```cpp
#include "system.h"

System greeniot;

// Initialize entire system (peripherals + sensors)
bool success = greeniot.init();

// Run measurement cycle (call from loop)
greeniot.run();

// Get system statistics
uint32_t uptime = greeniot.getUptime();           // milliseconds
uint32_t count = greeniot.getMeasurementCount();  // number of measurements

// Graceful shutdown
greeniot.stop();
```

### Peripheral Driver Layer

#### I2C Driver (`Drivers/i2c_driver.h`)

```cpp
// Initialize I2C
i2c_config_t config = { .sda_pin = 8, .scl_pin = 9, .frequency_hz = 100000, .timeout_ms = 100 };
i2c_init(&config);

// Write data
uint8_t data[] = {0x2C, 0x06};
i2c_write(0x44, data, 2);

// Read data
uint8_t buffer[6];
i2c_read(0x44, buffer, 6);

// Check device presence
if (i2c_device_present(0x44)) { /* device found */ }

// Scan bus
uint8_t devices[10];
uint8_t count = i2c_scan(devices, 10);
```

### HAL Sensor Layer

#### Sensor Interface (`HAL/Sensor/sensor_interface.h`)

```cpp
// Get sensor interface
const sensor_interface_t *sensor = sensor_get_interface("SHT31");

// Initialize
sensor->init();

// Configure
sensor_config_t config = { .precision = 2, /* ... */ };
sensor->configure(&config);

// Measure
sensor->trigger_measurement();
delay(15);  // Wait for measurement
sensor_data_t data;
sensor->read(&data);

// Access data
ESP_LOGI(TAG, "Temperature: %.2f°C", data.temperature_celsius);
ESP_LOGI(TAG, "Humidity: %.1f%%", data.humidity_percent);
ESP_LOGI(TAG, "Valid: %s", sensor_data_is_valid(&data) ? "Yes" : "No");

// Power management
sensor->sleep();
sensor->wakeup();
```

### Data Structures

#### `sensor_data_t`

```c
typedef struct {
    float temperature_celsius;   // Temperature in °C
    float humidity_percent;      // Relative humidity 0-100%
    uint32_t timestamp;          // Timestamp in milliseconds
    uint8_t quality_flags;       // Bit 7=temp_valid, Bit 6=hum_valid
} sensor_data_t;
```

#### `sensor_info_t`

```c
typedef struct {
    const char *name;            // e.g., "SHT31"
    const char *manufacturer;    // e.g., "Sensirion"
    const char *interface_type;  // e.g., "I2C"
    float temp_min, temp_max;    // Temperature range
    float hum_min, hum_max;      // Humidity range
    float temp_accuracy;         // ±°C
    float hum_accuracy;          // ±% RH
    uint16_t measurement_time_ms;
    uint16_t power_active_ua;    // Active current (µA)
    uint16_t power_sleep_ua;     // Sleep current (µA)
} sensor_info_t;
```

---

## 🧪 Testing

### Serial Monitor Test

After uploading, open serial monitor at **115200 baud**. You should see:

```
┌─────────────────────────────────────────┐
│ Measurement #1                          │
├─────────────────────────────────────────┤
│ Temperature:  23.45 °C                  │
│ Humidity:     65.3 %                    │
│ Timestamp:    12345 ms                  │
│ Temp Valid:   ✓ Yes                     │
│ Hum Valid:    ✓ Yes                     │
└─────────────────────────────────────────┘

Basil Growth Conditions Assessment:
───────────────────────────────────
  ✅ Temperature OPTIMAL - Ideal for basil
  ✅ Humidity OPTIMAL - Ideal for basil
```

### I2C Bus Scan

The firmware automatically scans the I2C bus on startup. Verify your sensor appears:

```
Scanning I2C bus...
Found 1 device(s):
  - 0x44  ← SHT31 detected
```

### Basil Growth Conditions

The firmware provides real-time assessment:

| Condition | Temperature | Humidity | Action |
|-----------|-------------|----------|--------|
| **Optimal** | 18-25°C | 60-70% | ✅ No action needed |
| **Suboptimal** | 15-18°C or 25-30°C | 40-60% or 70-80% | ⚠️ Monitor closely |
| **Critical** | <15°C or >30°C | <40% or >80% | 🚨 Immediate adjustment |

---

## 🔋 Power Optimization (Future Work)

### Current Consumption Estimate

| Mode | Current | Duration | Energy |
|------|---------|----------|--------|
| **Active (measure)** | ~5mA | 50ms | 0.07mAh |
| **BLE transmit** | ~15mA | 10ms | 0.04mAh |
| **Deep sleep** | 10µA | 299.94s | 0.83mAh |
| **Per cycle (5min)** | - | 300s | **0.94mAh** |

**Battery life** (2000mAh LiPo):
```
2000mAh / (0.94mAh × 12/hour × 24 hours) = ~7.4 months
```

### Optimization Strategies

1. **ESP32 Deep Sleep**: Use `esp_deep_sleep_start()` between measurements
2. **BLE Mesh**: Transmit only when data changes significantly
3. **Adaptive Sampling**: Increase interval when conditions stable
4. **Batch Transmission**: Buffer multiple readings before transmit

---

## 📝 Sampling Interval Justification

**Why 5 minutes?**

Basil (Ocimum basilicum) physiological response times:

1. **Temperature Stress Response**: 10-15 minutes to show signs
2. **Humidity Adaptation**: 5-10 minutes transpiration adjustment
3. **Photosynthesis**: Responds to microclimate within 5 minutes

**Scientific Basis:**
- *Chang et al. (2008)*: "Temperature fluctuations >2°C cause measurable stress"
- *Delfine et al. (2005)*: "Basil stomatal conductance adjusts within 5-8 minutes"

**Engineering Trade-offs:**
- **1 minute**: Excessive data, battery life ~2 months
- **5 minutes**: ✅ Optimal balance, battery life ~7 months
- **15 minutes**: Misses rapid microclimate changes

---

## 🤝 Contributing

### Code Style

- **C++ files**: Classes use `PascalCase`, functions use `camelCase()`
- **C files**: Use `snake_case` for functions and variables
- **Comments**: Doxygen format for public APIs
- **Indentation**: 4 spaces (no tabs)
- **Logging**: Use `ESP_LOGI`, `ESP_LOGE`, `ESP_LOGW` (not `Serial.print`)

### Adding Features

1. Fork repository
2. Create feature branch: `git checkout -b feature/new-sensor`
3. Follow layered architecture (don't violate layers!)
4. Keep `main.cpp` minimal - add logic to `System` class
5. Test thoroughly
6. Submit pull request

### Architecture Rules

❌ **DON'T**:
- Access `Wire.h` directly from application layer
- Put business logic in `main.cpp`
- Mix abstraction levels

✅ **DO**:
- Use peripheral drivers (`i2c_driver.h`) for hardware
- Put application logic in `System` class
- Keep layers strictly separated

---

## 📄 License

[Your License Here]

---

## 👤 Author

**GreenIoT Vertical Farming Project**
- Date: November 3, 2025
- Version: 2.1 (System Architecture)
- Platform: ESP32-C3 + PlatformIO + Arduino Framework
- Architecture: Layered (System Manager + HAL)

---

## 📖 References

1. Firmware Requirements Document (`01_Requirements/Firmware_Requirements.md`)
2. Sensirion SHT31 Datasheet
3. ESP32-C3 Technical Reference Manual
4. PlatformIO Documentation

---

**✨ Ready to monitor your vertical farm!** 🌱

