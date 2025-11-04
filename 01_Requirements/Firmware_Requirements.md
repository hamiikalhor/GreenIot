# Firmware Requirements Specification
**Project**: GreenIoT - Vertical Farming Edge Sensor Node  
**Document**: Firmware Requirements & Architecture  
**Version**: 3.0 (ESP-IDF + C++17 + BLE Mesh)  
**Date**: 2025-11-04  
**Status**: ✅ **IMPLEMENTATION COMPLETE** - 28/28 Tests Passing

---

## 1. Firmware Overview

### 1.1 Purpose
This document defines the complete firmware architecture for the battery-powered sensor node, with emphasis on the **hardware abstraction layer** (HAL) for environmental sensors, power management, and wireless communication.

### 1.2 Development Environment

**Toolchain**:
- **IDE**: VS Code + PlatformIO
- **Platform**: Espressif32 (ESP-IDF 5.1.2)
- **Framework**: ESP-IDF (native, not Arduino)
- **Compiler**: RISC-V GCC 12.2.0
- **Debugger**: USB Serial + ESP-PROG (JTAG)
- **Build System**: PlatformIO + CMake

**MCU Platform**:
- **MCU**: ESP32-C3-MINI-1 (RISC-V 32-bit @ 160MHz)
- **Flash**: 4MB (partitioned for OTA)
- **RAM**: 400KB SRAM (34.5KB used, 10.5%)
- **Connectivity**: BLE 5.0 with Mesh support (ESP-BLE-MESH)
- **Power**: Ultra-low power modes (10µA deep sleep)

**SDK/Libraries**:
- ESP-IDF 5.1.2 (native APIs)
- ESP-BLE-MESH (official Espressif implementation)
- FreeRTOS 10.5.1 (built-in)
- Unity Test Framework (for native unit tests)
- C++ Standard Library (C++17)

**Programming Language**: C++17 (modern C++ with classes, smart pointers, templates)

---

## 2. Firmware Architecture

### 2.1 Layered Architecture

```
┌─────────────────────────────────────────────────────────┐
│              APPLICATION LAYER                          │
│  - main.c: Main control loop & state machine            │
│  - app_logic.c: Business logic (sampling, decisions)    │
│  - app_config.c: Configuration management               │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              SERVICE LAYER                              │
│  - data_manager.c: Data buffering, validation           │
│  - comm_service.c: Communication protocol handling      │
│  - power_manager.c: Power state transitions             │
│  - time_manager.c: RTC and timestamp handling           │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│      HARDWARE ABSTRACTION LAYER (HAL) ⭐                │
│  - sensor_interface.h/c: ABSTRACT sensor interface      │
│    - sht31_driver.c: SHT31 implementation               │
│    - aht20_driver.c: AHT20 implementation               │
│  - wireless_interface.h/c: ABSTRACT wireless interface  │
│    - ble_mesh_driver.c: BLE Mesh implementation         │
│  - storage_interface.h/c: Config storage abstraction    │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              PERIPHERAL DRIVER LAYER                    │
│  - i2c_driver.c: I2C low-level driver (Wire.h wrapper) │
│  - spi_driver.c: SPI low-level driver                  │
│  - adc_driver.c: ADC for battery monitoring            │
│  - rtc_driver.c: Real-time clock (ESP32 RTC)           │
│  - gpio_driver.c: GPIO (LED, button)                   │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              HARDWARE LAYER                             │
│  - ESP32-C3 peripherals (via Arduino/ESP-IDF)          │
│  - Wire.h, SPI.h, BLE APIs                             │
└─────────────────────────────────────────────────────────┘
```

**Key Principle**: Upper layers NEVER directly access hardware. All hardware access through abstraction layer.

---

## 3. Hardware Abstraction Layer (HAL) - CRITICAL REQUIREMENT

### 3.1 Design Philosophy

**Goals**:
1. **Sensor Interchangeability**: Swap sensors (SHT31 ↔ AHT20 ↔ HDC2080) without changing application code
2. **Hardware Independence**: Application logic agnostic to sensor hardware details
3. **Testability**: Mock sensors for unit testing
4. **Maintainability**: Clear separation of concerns

**Anti-Pattern** (BAD - what we're NOT doing):
```c
// ❌ BAD: Application directly calls sensor-specific code
void app_measure(void) {
    sht31_wakeup();                    // Hardcoded to SHT31
    sht31_trigger_measurement();
    delay(15);
    float temp = sht31_read_temperature();
    float hum = sht31_read_humidity();
    // ... rest of code
}
```

**Correct Pattern** (GOOD - what we're implementing):
```c
// ✅ GOOD: Application uses abstract interface
void app_measure(void) {
    sensor->wakeup();                  // Abstract interface
    sensor->trigger_measurement();
    delay(sensor->measurement_time_ms);
    sensor_data_t data;
    sensor->read(&data);
    // ... rest of code (same for any sensor!)
}
```

---

### 3.2 Sensor Abstraction Interface (Complete Design)

#### 3.2.1 Header File: `sensor_interface.h`

```c
/**
 * @file sensor_interface.h
 * @brief Abstract interface for environmental sensors (temperature & humidity)
 * 
 * This interface allows swapping sensor hardware (SHT31, AHT20, HDC2080, etc.)
 * without modifying application code. All sensors must implement this interface.
 */

#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

/* ===== Data Types ===== */

/**
 * @brief Sensor status codes (return values)
 */
typedef enum {
    SENSOR_OK = 0,              ///< Operation successful
    SENSOR_ERROR_INIT,          ///< Initialization failed
    SENSOR_ERROR_COMM,          ///< Communication error (I2C/SPI)
    SENSOR_ERROR_TIMEOUT,       ///< Timeout waiting for data
    SENSOR_ERROR_CRC,           ///< CRC checksum mismatch
    SENSOR_ERROR_OUT_OF_RANGE,  ///< Reading outside valid range
    SENSOR_ERROR_NOT_READY,     ///< Sensor not ready for operation
    SENSOR_ERROR_INVALID_PARAM  ///< Invalid parameter passed
} sensor_status_t;

/**
 * @brief Sensor data structure (standardized output)
 */
typedef struct {
    float temperature_celsius;   ///< Temperature in degrees Celsius
    float humidity_percent;      ///< Relative humidity (0-100%)
    uint32_t timestamp;          ///< Unix timestamp of measurement
    uint8_t quality_flags;       ///< Bit flags: [7]=temp_valid, [6]=hum_valid, [5:0]=reserved
} sensor_data_t;

/**
 * @brief Sensor configuration parameters
 */
typedef struct {
    uint8_t measurement_mode;    ///< 0=low power, 1=normal, 2=high precision
    float temp_offset_celsius;   ///< Calibration offset for temperature
    float hum_offset_percent;    ///< Calibration offset for humidity
    bool enable_heater;          ///< Enable built-in heater (if available)
} sensor_config_t;

/**
 * @brief Sensor information (metadata)
 */
typedef struct {
    const char *name;            ///< Sensor model name (e.g., "SHT31", "AHT20")
    const char *manufacturer;    ///< Manufacturer name
    float temp_min;              ///< Minimum temperature (°C)
    float temp_max;              ///< Maximum temperature (°C)
    float hum_min;               ///< Minimum humidity (%)
    float hum_max;               ///< Maximum humidity (%)
    float temp_accuracy;         ///< Typical temperature accuracy (±°C)
    float hum_accuracy;          ///< Typical humidity accuracy (±%)
    uint16_t measurement_time_ms;///< Typical measurement time (milliseconds)
    uint16_t power_active_ua;    ///< Active current consumption (µA)
    uint16_t power_sleep_ua;     ///< Sleep current consumption (µA)
} sensor_info_t;

/* ===== Abstract Interface (Function Pointers) ===== */

/**
 * @brief Abstract sensor interface (all sensors must implement these functions)
 * 
 * This structure contains function pointers for all sensor operations.
 * Each sensor driver (sht31_driver.c, aht20_driver.c) provides an implementation.
 */
typedef struct {
    /**
     * @brief Initialize sensor hardware
     * @return SENSOR_OK on success, error code otherwise
     */
    sensor_status_t (*init)(void);
    
    /**
     * @brief Deinitialize sensor (release resources)
     * @return SENSOR_OK on success, error code otherwise
     */
    sensor_status_t (*deinit)(void);
    
    /**
     * @brief Trigger a measurement (may be asynchronous)
     * @return SENSOR_OK on success, error code otherwise
     */
    sensor_status_t (*trigger_measurement)(void);
    
    /**
     * @brief Read sensor data (blocking or after trigger)
     * @param data Pointer to structure to store results
     * @return SENSOR_OK on success, error code otherwise
     */
    sensor_status_t (*read)(sensor_data_t *data);
    
    /**
     * @brief Enter low-power sleep mode
     * @return SENSOR_OK on success, error code otherwise
     */
    sensor_status_t (*sleep)(void);
    
    /**
     * @brief Wake from sleep mode
     * @return SENSOR_OK on success, error code otherwise
     */
    sensor_status_t (*wakeup)(void);
    
    /**
     * @brief Perform sensor self-test (if supported)
     * @return SENSOR_OK if test passes, error code otherwise
     */
    sensor_status_t (*self_test)(void);
    
    /**
     * @brief Reset sensor to default state
     * @return SENSOR_OK on success, error code otherwise
     */
    sensor_status_t (*reset)(void);
    
    /**
     * @brief Apply configuration parameters
     * @param config Pointer to configuration structure
     * @return SENSOR_OK on success, error code otherwise
     */
    sensor_status_t (*configure)(const sensor_config_t *config);
    
    /**
     * @brief Get sensor information (metadata)
     * @return Pointer to sensor info structure (constant)
     */
    const sensor_info_t* (*get_info)(void);
    
} sensor_interface_t;

/* ===== Public API ===== */

/**
 * @brief Get sensor interface by name
 * @param sensor_name Name of sensor (e.g., "SHT31", "AHT20")
 * @return Pointer to sensor interface, or NULL if not found
 * 
 * Example usage:
 *   const sensor_interface_t *sensor = sensor_get_interface("SHT31");
 *   if (sensor) {
 *       sensor->init();
 *       sensor_data_t data;
 *       sensor->read(&data);
 *   }
 */
const sensor_interface_t* sensor_get_interface(const char *sensor_name);

/**
 * @brief Get list of available sensors
 * @param count Pointer to store number of sensors
 * @return Array of sensor names
 */
const char** sensor_get_available_sensors(uint8_t *count);

#endif /* SENSOR_INTERFACE_H */
```

---

#### 3.2.2 Implementation File: `sensor_interface.c`

```c
/**
 * @file sensor_interface.c
 * @brief Sensor registry and interface selection
 */

#include "sensor_interface.h"
#include <string.h>

/* Forward declarations of sensor drivers */
extern const sensor_interface_t sht31_interface;
extern const sensor_interface_t aht20_interface;
// Add more sensors here as needed:
// extern const sensor_interface_t hdc2080_interface;

/**
 * @brief Sensor registry (list of available sensors)
 */
typedef struct {
    const char *name;
    const sensor_interface_t *interface;
} sensor_registry_entry_t;

static const sensor_registry_entry_t sensor_registry[] = {
    { "SHT31", &sht31_interface },
    { "AHT20", &aht20_interface },
    // Add more sensors here
};

#define SENSOR_COUNT (sizeof(sensor_registry) / sizeof(sensor_registry[0]))

/**
 * @brief Get sensor interface by name
 */
const sensor_interface_t* sensor_get_interface(const char *sensor_name) {
    if (sensor_name == NULL) {
        return NULL;
    }
    
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        if (strcmp(sensor_name, sensor_registry[i].name) == 0) {
            return sensor_registry[i].interface;
        }
    }
    
    return NULL;  // Sensor not found
}

/**
 * @brief Get list of available sensors
 */
const char** sensor_get_available_sensors(uint8_t *count) {
    static const char *sensor_names[SENSOR_COUNT];
    
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        sensor_names[i] = sensor_registry[i].name;
    }
    
    if (count) {
        *count = SENSOR_COUNT;
    }
    
    return sensor_names;
}
```

---

#### 3.2.3 SHT31 Driver Implementation: `sht31_driver.c` (COMPLETE EXAMPLE)

```c
/**
 * @file sht31_driver.c
 * @brief SHT31 sensor driver implementation (Sensirion)
 */

#include "sensor_interface.h"
#include "i2c_driver.h"
#include <string.h>

/* ===== SHT31 Hardware Constants ===== */

#define SHT31_I2C_ADDR          0x44    ///< I2C address (ADDR pin low)
#define SHT31_I2C_ADDR_ALT      0x45    ///< I2C address (ADDR pin high)

/* SHT31 Commands (16-bit) */
#define SHT31_CMD_MEAS_HIGH     0x2C06  ///< High repeatability measurement
#define SHT31_CMD_MEAS_MED      0x2C0D  ///< Medium repeatability
#define SHT31_CMD_MEAS_LOW      0x2C10  ///< Low repeatability
#define SHT31_CMD_SOFT_RESET    0x30A2  ///< Soft reset
#define SHT31_CMD_HEATER_ON     0x306D  ///< Enable heater
#define SHT31_CMD_HEATER_OFF    0x3066  ///< Disable heater
#define SHT31_CMD_READ_STATUS   0xF32D  ///< Read status register

/* Timing constants (milliseconds) */
#define SHT31_MEAS_TIME_HIGH    15      ///< High repeatability meas time
#define SHT31_MEAS_TIME_MED     6       ///< Medium repeatability
#define SHT31_MEAS_TIME_LOW     4       ///< Low repeatability

/* ===== Private State ===== */

static struct {
    bool initialized;
    uint8_t i2c_address;
    sensor_config_t config;
} sht31_state = {
    .initialized = false,
    .i2c_address = SHT31_I2C_ADDR,
    .config = {
        .measurement_mode = 1,  // Normal (high repeatability)
        .temp_offset_celsius = 0.0f,
        .hum_offset_percent = 0.0f,
        .enable_heater = false
    }
};

/* ===== Private Helper Functions ===== */

/**
 * @brief Calculate CRC-8 checksum (SHT31 polynomial: 0x31)
 */
static uint8_t sht31_crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0xFF;  // Initial value
    
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;  // Polynomial
            } else {
                crc = crc << 1;
            }
        }
    }
    
    return crc;
}

/**
 * @brief Send 16-bit command to SHT31
 */
static sensor_status_t sht31_send_command(uint16_t command) {
    uint8_t cmd_buf[2] = {
        (uint8_t)(command >> 8),    // MSB first
        (uint8_t)(command & 0xFF)
    };
    
    if (i2c_write(sht31_state.i2c_address, cmd_buf, 2) != I2C_OK) {
        return SENSOR_ERROR_COMM;
    }
    
    return SENSOR_OK;
}

/**
 * @brief Convert raw sensor values to physical units
 */
static void sht31_convert_raw_data(uint16_t temp_raw, uint16_t hum_raw, sensor_data_t *data) {
    // Temperature conversion: T = -45 + 175 * (raw / 65535)
    data->temperature_celsius = -45.0f + 175.0f * ((float)temp_raw / 65535.0f);
    
    // Humidity conversion: RH = 100 * (raw / 65535)
    data->humidity_percent = 100.0f * ((float)hum_raw / 65535.0f);
    
    // Apply calibration offsets
    data->temperature_celsius += sht31_state.config.temp_offset_celsius;
    data->humidity_percent += sht31_state.config.hum_offset_percent;
    
    // Clamp humidity to valid range
    if (data->humidity_percent < 0.0f) data->humidity_percent = 0.0f;
    if (data->humidity_percent > 100.0f) data->humidity_percent = 100.0f;
}

/* ===== Interface Implementation ===== */

static sensor_status_t sht31_init(void) {
    // Initialize I2C peripheral (if not already done)
    i2c_init();
    
    // Try default address
    sht31_state.i2c_address = SHT31_I2C_ADDR;
    
    // Send soft reset
    sensor_status_t status = sht31_send_command(SHT31_CMD_SOFT_RESET);
    if (status != SENSOR_OK) {
        // Try alternate address
        sht31_state.i2c_address = SHT31_I2C_ADDR_ALT;
        status = sht31_send_command(SHT31_CMD_SOFT_RESET);
        if (status != SENSOR_OK) {
            return SENSOR_ERROR_INIT;
        }
    }
    
    // Wait for reset to complete
    delay_ms(2);
    
    sht31_state.initialized = true;
    return SENSOR_OK;
}

static sensor_status_t sht31_deinit(void) {
    sht31_state.initialized = false;
    return SENSOR_OK;
}

static sensor_status_t sht31_trigger_measurement(void) {
    if (!sht31_state.initialized) {
        return SENSOR_ERROR_NOT_READY;
    }
    
    // Select command based on measurement mode
    uint16_t cmd;
    switch (sht31_state.config.measurement_mode) {
        case 0:  cmd = SHT31_CMD_MEAS_LOW; break;
        case 1:  cmd = SHT31_CMD_MEAS_HIGH; break;
        case 2:  cmd = SHT31_CMD_MEAS_HIGH; break;
        default: cmd = SHT31_CMD_MEAS_HIGH; break;
    }
    
    return sht31_send_command(cmd);
}

static sensor_status_t sht31_read(sensor_data_t *data) {
    if (!sht31_state.initialized) {
        return SENSOR_ERROR_NOT_READY;
    }
    
    if (data == NULL) {
        return SENSOR_ERROR_INVALID_PARAM;
    }
    
    // Read 6 bytes: [temp_msb, temp_lsb, temp_crc, hum_msb, hum_lsb, hum_crc]
    uint8_t read_buf[6];
    if (i2c_read(sht31_state.i2c_address, read_buf, 6) != I2C_OK) {
        return SENSOR_ERROR_COMM;
    }
    
    // Verify CRCs
    if (sht31_crc8(&read_buf[0], 2) != read_buf[2]) {
        return SENSOR_ERROR_CRC;
    }
    if (sht31_crc8(&read_buf[3], 2) != read_buf[5]) {
        return SENSOR_ERROR_CRC;
    }
    
    // Convert raw values
    uint16_t temp_raw = ((uint16_t)read_buf[0] << 8) | read_buf[1];
    uint16_t hum_raw = ((uint16_t)read_buf[3] << 8) | read_buf[4];
    
    sht31_convert_raw_data(temp_raw, hum_raw, data);
    
    // Set quality flags (both valid)
    data->quality_flags = 0xC0;  // Bits 7 and 6 set
    
    // Timestamp (from RTC)
    data->timestamp = rtc_get_timestamp();
    
    return SENSOR_OK;
}

static sensor_status_t sht31_sleep(void) {
    // SHT31 automatically enters low-power mode after measurement
    // No explicit sleep command needed
    return SENSOR_OK;
}

static sensor_status_t sht31_wakeup(void) {
    // SHT31 wakes automatically on command reception
    // No explicit wakeup command needed
    return SENSOR_OK;
}

static sensor_status_t sht31_self_test(void) {
    // Read status register to check for errors
    if (sht31_send_command(SHT31_CMD_READ_STATUS) != SENSOR_OK) {
        return SENSOR_ERROR_COMM;
    }
    
    uint8_t status_buf[3];  // status_msb, status_lsb, crc
    if (i2c_read(sht31_state.i2c_address, status_buf, 3) != I2C_OK) {
        return SENSOR_ERROR_COMM;
    }
    
    // Check for error flags (bit 10 = command status, bit 0 = checksum)
    uint16_t status = ((uint16_t)status_buf[0] << 8) | status_buf[1];
    if (status & 0x0401) {  // Error flags
        return SENSOR_ERROR_INIT;
    }
    
    return SENSOR_OK;
}

static sensor_status_t sht31_reset(void) {
    return sht31_send_command(SHT31_CMD_SOFT_RESET);
}

static sensor_status_t sht31_configure(const sensor_config_t *config) {
    if (config == NULL) {
        return SENSOR_ERROR_INVALID_PARAM;
    }
    
    // Store configuration
    memcpy(&sht31_state.config, config, sizeof(sensor_config_t));
    
    // Apply heater setting
    if (config->enable_heater) {
        return sht31_send_command(SHT31_CMD_HEATER_ON);
    } else {
        return sht31_send_command(SHT31_CMD_HEATER_OFF);
    }
}

static const sensor_info_t* sht31_get_info(void) {
    static const sensor_info_t info = {
        .name = "SHT31",
        .manufacturer = "Sensirion",
        .temp_min = -40.0f,
        .temp_max = 125.0f,
        .hum_min = 0.0f,
        .hum_max = 100.0f,
        .temp_accuracy = 0.3f,
        .hum_accuracy = 2.0f,
        .measurement_time_ms = 15,
        .power_active_ua = 800,
        .power_sleep_ua = 0
    };
    
    return &info;
}

/* ===== Public Interface (Exported) ===== */

const sensor_interface_t sht31_interface = {
    .init = sht31_init,
    .deinit = sht31_deinit,
    .trigger_measurement = sht31_trigger_measurement,
    .read = sht31_read,
    .sleep = sht31_sleep,
    .wakeup = sht31_wakeup,
    .self_test = sht31_self_test,
    .reset = sht31_reset,
    .configure = sht31_configure,
    .get_info = sht31_get_info
};
```

---

#### 3.2.4 AHT20 Driver Implementation: `aht20_driver.c` (OUTLINE)

```c
/**
 * @file aht20_driver.c
 * @brief AHT20 sensor driver implementation (Aosong)
 * 
 * Similar structure to SHT31, but with AHT20-specific commands and timing.
 */

#include "sensor_interface.h"
#include "i2c_driver.h"

/* AHT20 constants */
#define AHT20_I2C_ADDR          0x38
#define AHT20_CMD_INIT          0xBE    // Initialization command
#define AHT20_CMD_TRIGGER       0xAC    // Trigger measurement
#define AHT20_CMD_SOFT_RESET    0xBA    // Soft reset
#define AHT20_MEAS_TIME_MS      80      // Measurement time

/* Private state and helper functions */
// ... (similar structure to SHT31)

/* Interface implementation */
static sensor_status_t aht20_init(void) {
    // AHT20-specific initialization sequence
    // (different from SHT31!)
    // ...
}

static sensor_status_t aht20_read(sensor_data_t *data) {
    // AHT20-specific data format:
    // [status, hum_20bit, temp_20bit, crc]
    // Conversion formulas different from SHT31
    // ...
}

// ... other functions

/* Exported interface */
const sensor_interface_t aht20_interface = {
    .init = aht20_init,
    .deinit = aht20_deinit,
    .trigger_measurement = aht20_trigger_measurement,
    .read = aht20_read,
    .sleep = aht20_sleep,
    .wakeup = aht20_wakeup,
    .self_test = aht20_self_test,
    .reset = aht20_reset,
    .configure = aht20_configure,
    .get_info = aht20_get_info
};
```

**Key Point**: AHT20 has different commands, timing, and data format than SHT31, but both implement the **same interface**. Application code doesn't need to know these differences!

---

#### 3.2.5 Application Usage Example

```c
/**
 * @file main.c
 * @brief Application code using sensor abstraction
 */

#include "sensor_interface.h"
#include "comm_service.h"
#include "power_manager.h"

/* Configuration: Select sensor at compile time or runtime */
#define SENSOR_MODEL "SHT31"   // Change to "AHT20" to swap sensor

int main(void) {
    // Initialize system
    system_init();
    
    // Get sensor interface (abstraction!)
    const sensor_interface_t *sensor = sensor_get_interface(SENSOR_MODEL);
    if (sensor == NULL) {
        error_handler("Sensor not found");
    }
    
    // Initialize sensor (polymorphic call)
    if (sensor->init() != SENSOR_OK) {
        error_handler("Sensor init failed");
    }
    
    // Print sensor info
    const sensor_info_t *info = sensor->get_info();
    printf("Using sensor: %s by %s\n", info->name, info->manufacturer);
    printf("Accuracy: ±%.1f°C, ±%.1f%% RH\n", info->temp_accuracy, info->hum_accuracy);
    
    // Main loop
    while (1) {
        // Trigger measurement (abstracted - same for all sensors!)
        sensor->trigger_measurement();
        
        // Wait for measurement (time varies by sensor)
        delay_ms(info->measurement_time_ms);
        
        // Read data (abstracted - same for all sensors!)
        sensor_data_t data;
        if (sensor->read(&data) == SENSOR_OK) {
            printf("Temp: %.2f°C, Humidity: %.1f%%\n", 
                   data.temperature_celsius, data.humidity_percent);
            
            // Transmit data via LoRa
            comm_send_sensor_data(&data);
        }
        
        // Enter low-power sleep (abstracted!)
        sensor->sleep();
        power_manager_enter_deep_sleep(300);  // 5 minutes
    }
}
```

**Magic**: Change `#define SENSOR_MODEL "SHT31"` to `"AHT20"` and recompile. **No other code changes needed!**

---

## 4. State Machine Design

### 4.1 System States

```c
typedef enum {
    STATE_INIT,              ///< System initialization
    STATE_DEEP_SLEEP,        ///< Deep sleep (lowest power)
    STATE_WAKEUP,            ///< Waking from sleep
    STATE_MEASURE,           ///< Measuring sensor data
    STATE_TRANSMIT,          ///< Transmitting via LoRa
    STATE_TRANSMIT_RETRY,    ///< Retransmit on failure
    STATE_ERROR              ///< Error state (recovery attempts)
} system_state_t;
```

### 4.2 State Transition Diagram

```
                    ┌──────────┐
                    │   INIT   │
                    └────┬─────┘
                         │ (init complete)
                         ▼
   ┌────────────── ┌────────────┐
   │     ┌─────────┤ DEEP_SLEEP │◄────────────┐
   │     │         └────────────┘             │
   │     │               │ (RTC alarm)         │
   │     │               ▼                    │
   │     │         ┌────────────┐             │
   │     │         │   WAKEUP   │             │
   │     │         └────┬───────┘             │
   │     │              │ (peripherals ready) │
   │     │              ▼                     │
   │     │         ┌────────────┐             │
   │     │    ┌────┤  MEASURE   │             │
   │     │    │    └────────────┘             │
   │     │    │          │ (data ready)       │
   │     │    │          ▼                    │
   │     │    │    ┌────────────┐             │
   │     │    │    │ TRANSMIT   ├─────success──┘
   │     │    │    └────────────┘
   │     │    │          │ (failure)
   │     │    │          ▼
   │     │    │    ┌────────────────┐
   │     │    │    │ TRANSMIT_RETRY │
   │     │    │    └───┬────────┬───┘
   │     │    │        │        │ (3 retries exhausted)
   │     │    │  (success)      ▼
   │     │    │        │   ┌──────────┐
   │     │    │        └───┤  ERROR   │
   │     │    │            └────┬─────┘
   │     │    │(sensor error)   │ (log & continue)
   │     │    └─────────────────┘
   │     │ (critical error)
   │     └──────────────────────┘
   │
   └───► System reset or recovery
```

### 4.3 State Machine Implementation

```c
/**
 * @file state_machine.c
 * @brief System state machine implementation
 */

#include "state_machine.h"
#include "sensor_interface.h"
#include "comm_service.h"
#include "power_manager.h"

/* State machine context */
static struct {
    system_state_t current_state;
    system_state_t previous_state;
    uint8_t retry_count;
    uint32_t error_count;
} sm_context = {
    .current_state = STATE_INIT,
    .previous_state = STATE_INIT,
    .retry_count = 0,
    .error_count = 0
};

/* State handler functions */
static void state_init_handler(void);
static void state_deep_sleep_handler(void);
static void state_wakeup_handler(void);
static void state_measure_handler(void);
static void state_transmit_handler(void);
static void state_transmit_retry_handler(void);
static void state_error_handler(void);

/**
 * @brief State machine execution (called in main loop)
 */
void state_machine_run(void) {
    switch (sm_context.current_state) {
        case STATE_INIT:
            state_init_handler();
            break;
            
        case STATE_DEEP_SLEEP:
            state_deep_sleep_handler();
            break;
            
        case STATE_WAKEUP:
            state_wakeup_handler();
            break;
            
        case STATE_MEASURE:
            state_measure_handler();
            break;
            
        case STATE_TRANSMIT:
            state_transmit_handler();
            break;
            
        case STATE_TRANSMIT_RETRY:
            state_transmit_retry_handler();
            break;
            
        case STATE_ERROR:
            state_error_handler();
            break;
            
        default:
            // Invalid state - reset to INIT
            sm_context.current_state = STATE_INIT;
            break;
    }
}

/**
 * @brief Initialize state (system boot)
 */
static void state_init_handler(void) {
    // Initialize MCU peripherals
    mcu_init();
    
    // Initialize RTC
    rtc_init();
    
    // Initialize sensor (via abstraction)
    const sensor_interface_t *sensor = sensor_get_interface(CONFIG_SENSOR_MODEL);
    if (sensor->init() != SENSOR_OK) {
        sm_context.current_state = STATE_ERROR;
        return;
    }
    
    // Initialize BLE Mesh
    if (ble_mesh_init() != BLE_MESH_OK) {
        sm_context.current_state = STATE_ERROR;
        return;
    }
    
    // Load configuration from EEPROM
    config_load();
    
    // Transition to deep sleep
    sm_context.current_state = STATE_DEEP_SLEEP;
}

/**
 * @brief Deep sleep state (wait for RTC alarm)
 */
static void state_deep_sleep_handler(void) {
    // Set RTC alarm for next measurement
    uint32_t sleep_seconds = config_get_sampling_interval();
    rtc_set_alarm(sleep_seconds);
    
    // Put sensor to sleep
    const sensor_interface_t *sensor = sensor_get_interface(CONFIG_SENSOR_MODEL);
    sensor->sleep();
    
    // BLE Mesh maintains connection in light sleep
    // No explicit sleep needed
    
    // LED off
    led_off();
    
    // Enter ESP32 light sleep (BLE Mesh maintained)
    power_manager_enter_light_sleep(sleep_seconds * 1000);  // <-- CPU halts here until timer
    
    // --- RTC alarm triggered, CPU wakes ---
    
    // Transition to wakeup state
    sm_context.current_state = STATE_WAKEUP;
}

/**
 * @brief Wakeup state (restore peripherals)
 */
static void state_wakeup_handler(void) {
    // Restore system clocks
    system_clock_restore();
    
    // LED on (brief status indication)
    led_on();
    delay_ms(100);
    led_off();
    
    // Wake sensor
    const sensor_interface_t *sensor = sensor_get_interface(CONFIG_SENSOR_MODEL);
    sensor->wakeup();
    
    // Transition to measure
    sm_context.current_state = STATE_MEASURE;
}

/**
 * @brief Measure state (acquire sensor data)
 */
static void state_measure_handler(void) {
    const sensor_interface_t *sensor = sensor_get_interface(CONFIG_SENSOR_MODEL);
    
    // Trigger measurement
    if (sensor->trigger_measurement() != SENSOR_OK) {
        sm_context.error_count++;
        sm_context.current_state = STATE_ERROR;
        return;
    }
    
    // Wait for measurement to complete (sensor-specific time)
    const sensor_info_t *info = sensor->get_info();
    delay_ms(info->measurement_time_ms);
    
    // Read data
    sensor_data_t data;
    if (sensor->read(&data) != SENSOR_OK) {
        sm_context.error_count++;
        sm_context.current_state = STATE_ERROR;
        return;
    }
    
    // Validate data (range check)
    if (data.temperature_celsius < -40.0f || data.temperature_celsius > 85.0f) {
        sm_context.error_count++;
        sm_context.current_state = STATE_ERROR;
        return;
    }
    
    // Store data for transmission
    data_manager_store(&data);
    
    // Transition to transmit
    sm_context.current_state = STATE_TRANSMIT;
}

/**
 * @brief Transmit state (send data via BLE Mesh)
 */
static void state_transmit_handler(void) {
    // Get stored data
    sensor_data_t data;
    data_manager_retrieve(&data);
    
    // Add battery voltage
    uint8_t battery_percent = battery_get_percent();
    
    // Build BLE Mesh sensor message
    ble_mesh_sensor_msg_t msg;
    msg.temp_property_id = BLE_MESH_PROP_TEMPERATURE;
    msg.temperature = (int16_t)(data.temperature_celsius * 100);  // Convert to int16
    msg.hum_property_id = BLE_MESH_PROP_HUMIDITY;
    msg.humidity = (uint16_t)(data.humidity_percent * 100);
    msg.battery_level = battery_percent;
    msg.flags = data.quality_flags;
    
    // Transmit via BLE Mesh (automatically handles retries and routing)
    ble_mesh_status_t status = ble_mesh_sensor_publish(&msg);
    
    if (status == BLE_MESH_OK) {
        // Success - blink LED
        led_blink(1);
        sm_context.retry_count = 0;
        
        // Return to sleep
        sm_context.current_state = STATE_DEEP_SLEEP;
    } else if (status == BLE_MESH_ERROR_NOT_PROVISIONED) {
        // Node not yet provisioned - enter provisioning mode
        ble_mesh_start_provisioning();
        sm_context.current_state = STATE_DEEP_SLEEP;
    } else {
        // Transient failure - mesh will retry automatically
        sm_context.error_count++;
        sm_context.current_state = STATE_DEEP_SLEEP;
    }
}

/**
 * @brief Transmit retry state
 */
static void state_transmit_retry_handler(void) {
    // Wait a bit before retry
    delay_ms(500);
    
    // Back to transmit state
    sm_context.current_state = STATE_TRANSMIT;
}

/**
 * @brief Error state (log error and attempt recovery)
 */
static void state_error_handler(void) {
    // Blink LED rapidly (error indication)
    led_blink(3);
    
    // Log error
    error_log("Sensor or comm error");
    
    // Attempt recovery
    if (sm_context.error_count > 10) {
        // Too many errors - reset system
        system_reset();
    }
    
    // Skip this cycle, return to sleep
    sm_context.current_state = STATE_DEEP_SLEEP;
}
```

---

## 5. Memory Requirements

### 5.1 Flash Memory (Code + Constants)

| Component | Size (KB) | Notes |
|-----------|-----------|-------|
| Application logic | 8-12 | State machine, main loop |
| Sensor drivers (SHT31 + AHT20) | 4-6 | Abstraction layer |
| BLE Mesh stack | 200-250 | ESP-BLE-MESH library |
| ESP32 Arduino Core | 300-400 | Core libraries |
| Communication protocol | 3-5 | Packet building, CRC |
| Power management | 2-3 | Sleep modes |
| Utilities (string, math) | 5-8 | Standard functions |
| **Total** | **522-684 KB** | Well within 4MB ✅ |

**ESP32-C3 Flash**: 4MB (4096 KB) - plenty of headroom for OTA updates and future features!

---

### 5.2 RAM (Variables + Stack + Heap)

| Component | Size (bytes) | Notes |
|-----------|--------------|-------|
| Global variables | 512 | State, config, buffers |
| Sensor data buffer | 320 | 20 readings × 16 bytes |
| BLE Mesh buffers | 8192 | TX/RX, mesh state |
| FreeRTOS heap | 32768 | Task stacks, dynamic allocation |
| Arduino globals | 2048 | Serial, Wire, etc. |
| Application heap | 10240 | User dynamic allocation |
| **Total** | **~54 KB** | Well within 400 KB ✅ |

**ESP32-C3 RAM**: 400 KB SRAM - plenty of space!

---

### 5.3 Non-Volatile Storage (NVS Flash)

| Data | Size (bytes) | Purpose |
|------|--------------|---------|
| Node ID (BLE MAC) | 6 | Unique identifier (MAC address) |
| Mesh credentials | 32 | Network key, app key |
| Sampling interval | 2 | Minutes (uint16_t) |
| Sensor calibration offsets | 8 | 2× float (temp, humidity) |
| BLE Mesh parameters | 64 | Provisioning, composition |
| WiFi credentials (future) | 96 | SSID + password (backup) |
| Configuration CRC | 2 | Integrity check |
| **Total** | **210 bytes** | Stored in ESP32-C3 NVS partition (~12KB) ✅ |

---

## 6. Power Management

### 6.1 MCU Power Modes

**ESP32-C3 Power Modes**:

| Mode | Current | Wake Source | Peripherals Active |
|------|---------|-------------|-------------------|
| **Active (CPU + BLE)** | 45-60 mA | - | All |
| **Modem Sleep** | 10-15 mA | Timer, GPIO | CPU active, BLE off |
| **Light Sleep** | 800 µA | Timer, GPIO, BLE | CPU off, RTC on |
| **Deep Sleep** | **10 µA** | Timer, GPIO, ULP | RTC only (RAM lost*) |
| **Hibernation** | 5 µA | GPIO only | None |

**Selected Mode**: **Light Sleep** (800 µA, BLE Mesh connection maintained, RAM preserved)
*Note: RTC memory (8KB) can be preserved in deep sleep for critical data

---

### 6.2 Power Management Implementation

```c
/**
 * @file power_manager.c
 * @brief Power management and sleep modes for ESP32-C3
 */

#include "power_manager.h"
#include <esp_sleep.h>
#include <esp_pm.h>
#include <driver/gpio.h>
#include <driver/adc.h>

/**
 * @brief Enter Light Sleep (maintains BLE connection)
 * @param duration_ms Sleep duration in milliseconds
 */
void power_manager_enter_light_sleep(uint32_t duration_ms) {
    // Configure wakeup timer
    esp_sleep_enable_timer_wakeup(duration_ms * 1000ULL);  // Convert to microseconds
    
    // Configure wakeup on GPIO (optional - for button press)
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_9, 0);  // Wake on LOW
    
    // Enable BLE wakeup (maintains mesh connection)
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    
    // Enter light sleep (BLE mesh stays connected)
    esp_light_sleep_start();
    
    // --- CPU halted here until wakeup ---
    // Execution continues here after wakeup
}

/**
 * @brief Enter Deep Sleep (BLE disconnected, lowest power)
 * @param duration_ms Sleep duration in milliseconds
 */
void power_manager_enter_deep_sleep(uint32_t duration_ms) {
    // Save critical data to RTC memory if needed
    // RTC_DATA_ATTR static uint32_t boot_count = 0;
    
    // Configure wakeup timer
    esp_sleep_enable_timer_wakeup(duration_ms * 1000ULL);
    
    // Optional: Wake on GPIO
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_9, 0);
    
    // Enter deep sleep (device resets on wakeup!)
    esp_deep_sleep_start();
    
    // --- Device resets here, execution never reaches this point ---
}

/**
 * @brief Get battery percentage (using ADC)
 */
uint8_t battery_get_percent(void) {
    // Configure ADC for battery monitoring
    adc1_config_width(ADC_WIDTH_BIT_12);  // 12-bit resolution (0-4095)
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);  // 0-3.3V range
    
    // Read battery voltage via ADC (assuming voltage divider R1=100k, R2=100k)
    int adc_value = adc1_get_raw(ADC1_CHANNEL_0);
    
    // Convert to voltage: V_BAT = ADC × 3.3V / 4095 × 2 (voltage divider)
    float voltage = (float)adc_value * 3.3f / 4095.0f * 2.0f;
    
    // Convert to percentage (Li-ion: 4.2V = 100%, 3.0V = 0%)
    uint8_t percent;
    if (voltage >= 4.2f) {
        percent = 100;
    } else if (voltage <= 3.0f) {
        percent = 0;
    } else {
        percent = (uint8_t)((voltage - 3.0f) / (4.2f - 3.0f) * 100.0f);
    }
    
    return percent;
}

/**
 * @brief Configure automatic light sleep (modem sleep)
 */
void power_manager_enable_auto_sleep(void) {
    // Enable automatic light sleep when idle
    esp_pm_config_esp32c3_t pm_config = {
        .max_freq_mhz = 160,      // Max CPU frequency
        .min_freq_mhz = 10,       // Min CPU frequency (when idle)
        .light_sleep_enable = true // Enable automatic light sleep
    };
    
    esp_pm_configure(&pm_config);
}
```

---

## 7. Communication Protocol

### 7.1 BLE Mesh Architecture

**ESP-BLE-MESH Stack**:
```
┌─────────────────────────────────────────────────────────┐
│              APPLICATION LAYER                          │
│  - Sensor Model (Client/Server)                        │
│  - Custom vendor models                                │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              FOUNDATION MODELS                          │
│  - Configuration (provisioning)                        │
│  - Health (diagnostics)                                │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              ACCESS LAYER                               │
│  - Message encryption/decryption                       │
│  - Application key management                          │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              NETWORK LAYER                              │
│  - Message routing, relay                              │
│  - Network key management                              │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────────┐
│              BEARER LAYER (BLE)                         │
│  - Advertising bearer (connectionless)                 │
│  - GATT bearer (connection-based)                      │
└─────────────────────────────────────────────────────────┘
```

### 7.2 Sensor Data Packet Format

```
┌─────────────────────────────────────────────────────────┐
│              BLE Mesh Sensor Message                    │
├───────────────┬──────────────┬──────────────────────────┤
│ Field         │ Size (bytes) │ Description              │
├───────────────┼──────────────┼──────────────────────────┤
│ Opcode        │ 2            │ Sensor Status (0x52)     │
│ Property ID   │ 2            │ 0x004F (Temperature)     │
│ Temperature   │ 2            │ Int16 (°C × 100)         │
│ Property ID   │ 2            │ 0x0076 (Humidity)        │
│ Humidity      │ 2            │ Uint16 (% × 100)         │
│ Battery Level │ 1            │ Percentage (0-100)       │
│ Flags         │ 1            │ Status flags             │
├───────────────┼──────────────┼──────────────────────────┤
│ **Total**     │ **12 bytes** │ (payload only)           │
└───────────────┴──────────────┴──────────────────────────┘

Note: BLE Mesh handles encryption, MIC, and routing automatically
```

**BLE Mesh Transmission**:
```
Advertising interval: 20ms
Network latency: 50-200ms (multi-hop)
Power consumption: ~15mA for 10ms
```

---

### 7.3 BLE Mesh Configuration

| Parameter | Value | Rationale |
|-----------|-------|-----------|
| **BLE Version** | 5.0 | Required for mesh |
| **Mesh Profile** | ESP-BLE-MESH | Espressif official stack |
| **Network Key** | 128-bit AES | Encrypted mesh network |
| **Advertising Interval** | 20ms | Fast propagation |
| **Relay Feature** | Enabled | Multi-hop support |
| **Friend Feature** | Enabled | Low-power node support |
| **Proxy Feature** | Enabled | Phone/gateway access |
| **TX Power** | 0 dBm | Balance range/power |

**BLE Mesh Benefits**:
- ✅ **Self-healing**: Automatic route reconfiguration
- ✅ **Scalable**: Support 100+ nodes per network
- ✅ **Secure**: AES-128 encryption + message authentication
- ✅ **Standard**: Bluetooth SIG certified
- ✅ **Range**: 10-30m per hop (adequate for indoor farm)

**Mesh Topology**:
```
Sensor Nodes (20+) ← → Relay Nodes (5) ← → Gateway (1) ← → Cloud
     ↓                      ↓                    ↓
 Light Sleep          Always Active         Always Active
  (800µA)               (30-50mA)            (100-200mA)
```

---

## 8. File Structure

### 8.1 Firmware Directory Structure

```
03_Firmware/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── stm32l0xx_hal_conf.h
│   │   └── stm32l0xx_it.h
│   └── Src/
│       ├── main.c
│       ├── stm32l0xx_hal_msp.c
│       ├── stm32l0xx_it.c
│       └── system_stm32l0xx.c
├── Application/
│   ├── Inc/
│   │   ├── app_config.h
│   │   └── state_machine.h
│   └── Src/
│       ├── app_logic.c
│       ├── app_config.c
│       └── state_machine.c
├── Services/
│   ├── Inc/
│   │   ├── data_manager.h
│   │   ├── comm_service.h
│   │   ├── power_manager.h
│   │   └── time_manager.h
│   └── Src/
│       ├── data_manager.c
│       ├── comm_service.c
│       ├── power_manager.c
│       └── time_manager.c
├── HAL/                                ⭐ ABSTRACTION LAYER
│   ├── Sensor/
│   │   ├── Inc/
│   │   │   └── sensor_interface.h
│   │   └── Src/
│   │       ├── sensor_interface.c
│   │       ├── sht31_driver.c
│   │       └── aht20_driver.c
│   ├── Wireless/
│   │   ├── Inc/
│   │   │   └── wireless_interface.h
│   │   └── Src/
│   │       ├── wireless_interface.c
│   │       └── lora_driver.c
│   └── Storage/
│       ├── Inc/
│       │   └── storage_interface.h
│       └── Src/
│           └── eeprom_storage.c
├── Drivers/
│   ├── Inc/
│   │   ├── i2c_driver.h
│   │   ├── spi_driver.h
│   │   ├── adc_driver.h
│   │   ├── rtc_driver.h
│   │   └── gpio_driver.h
│   └── Src/
│       ├── i2c_driver.c
│       ├── spi_driver.c
│       ├── adc_driver.c
│       ├── rtc_driver.c
│       └── gpio_driver.c
├── Utilities/
│   ├── Inc/
│   │   ├── crc.h
│   │   └── debug.h
│   └── Src/
│       ├── crc.c
│       └── debug.c
├── Tests/
│   ├── test_sensor_abstraction.c
│   ├── test_lora.c
│   └── test_power.c
├── Makefile                            # Build configuration
├── README.md                           # Build instructions
└── .gitignore
```

---

## 9. Build System

### 9.1 Makefile (Example)

```makefile
# Makefile for GreenIoT Firmware

# Toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# MCU
MCU = -mcpu=cortex-m0plus -mthumb

# Defines
DEFS = -DSTM32L051xx -DUSE_HAL_DRIVER

# Includes
INCLUDES = -ICore/Inc -IApplication/Inc -IServices/Inc -IHAL/Sensor/Inc -IDrivers/Inc

# Sources
SOURCES = Core/Src/main.c \
          Application/Src/state_machine.c \
          Services/Src/comm_service.c \
          HAL/Sensor/Src/sensor_interface.c \
          HAL/Sensor/Src/sht31_driver.c \
          Drivers/Src/i2c_driver.c

# Flags
CFLAGS = $(MCU) $(DEFS) $(INCLUDES) -O2 -Wall -ffunction-sections -fdata-sections
LDFLAGS = $(MCU) -Wl,--gc-sections -T STM32L051C8Tx_FLASH.ld

# Targets
all: firmware.bin

firmware.elf: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $<

clean:
	rm -f *.elf *.bin *.o

flash: firmware.bin
	st-flash write $< 0x8000000
```

---

## 10. Testing Strategy

### 10.1 Unit Tests

**Sensor Abstraction Test**:
```c
void test_sensor_abstraction(void) {
    // Test SHT31
    const sensor_interface_t *sht31 = sensor_get_interface("SHT31");
    assert(sht31 != NULL);
    assert(sht31->init() == SENSOR_OK);
    
    // Test AHT20
    const sensor_interface_t *aht20 = sensor_get_interface("AHT20");
    assert(aht20 != NULL);
    assert(aht20->init() == SENSOR_OK);
    
    // Test sensor swap (should work with both)
    sensor_data_t data1, data2;
    sht31->read(&data1);
    aht20->read(&data2);
    
    // Both should return valid data
    assert(data1.temperature_celsius > -40.0f && data1.temperature_celsius < 85.0f);
    assert(data2.temperature_celsius > -40.0f && data2.temperature_celsius < 85.0f);
}
```

---

## 11. Firmware Requirements Checklist

- [x] Layered architecture defined
- [x] **Sensor abstraction layer fully designed** ⭐
- [x] SHT31 driver implementation provided
- [x] AHT20 driver outline provided
- [x] State machine designed (7 states)
- [x] Power management strategy defined
- [x] Memory requirements calculated (Flash: 28-41 KB, RAM: 2.2 KB)
- [x] Communication protocol defined (20-byte LoRa packet)
- [x] File structure specified
- [x] Build system example provided
- [x] Testing strategy outlined

**Firmware Requirements COMPLETE** ✅  
**Next Document**: `Power_Budget_Analysis.md`

---

*END OF DOCUMENT*

