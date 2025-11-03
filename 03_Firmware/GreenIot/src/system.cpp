/**
 * @file system.cpp
 * @brief System Manager Implementation
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#include "system.h"
#include <Arduino.h>
#include "Drivers/i2c_driver.h"
#include "esp_log.h"

// ============================================================================
// Configuration
// ============================================================================

static const char *TAG = "System";

// Sensor Selection
#define SENSOR_MODEL "SHT31"

// I2C Configuration
#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 9
#define I2C_FREQUENCY_HZ 100000
#define I2C_TIMEOUT_MS 100

// Sampling interval - 5 minutes
#define SAMPLING_INTERVAL_MS 300000

// ============================================================================
// Public Methods
// ============================================================================

System::System() 
    : sensor(nullptr)
    , measurement_count(0)
    , last_measurement_time(0)
    , initialized(false)
{
}

System::~System() {
    stop();
}

bool System::init() {
    ESP_LOGI(TAG, "\n╔══════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║   GreenIoT Vertical Farming Project      ║");
    ESP_LOGI(TAG, "║   Environmental Monitoring Sensor Node   ║");
    ESP_LOGI(TAG, "║   Layered Architecture Implementation    ║");
    ESP_LOGI(TAG, "╚══════════════════════════════════════════╝\n");
    
    ESP_LOGI(TAG, "Architecture Layers:");
    ESP_LOGI(TAG, "  [4] Application Layer    ← System Manager");
    ESP_LOGI(TAG, "  [3] HAL Layer            ← sensor_interface");
    ESP_LOGI(TAG, "  [2] Peripheral Driver    ← i2c_driver");
    ESP_LOGI(TAG, "  [1] Hardware Layer       ← ESP32-C3 / Wire.h\n");
    
    // Initialize peripherals
    if (!initPeripherals()) {
        ESP_LOGE(TAG, "Peripheral initialization failed");
        return false;
    }
    
    // Initialize sensor
    if (!initSensor()) {
        ESP_LOGE(TAG, "Sensor initialization failed");
        return false;
    }
    
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    ESP_LOGI(TAG, "Sampling Interval: %lu seconds (%.1f minutes)", 
             SAMPLING_INTERVAL_MS / 1000, SAMPLING_INTERVAL_MS / 60000.0f);
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    // Perform initial measurement
    ESP_LOGI(TAG, "Starting measurements...\n");
    performMeasurement();
    
    initialized = true;
    return true;
}

void System::run() {
    if (!initialized) {
        ESP_LOGE(TAG, "System not initialized!");
        return;
    }
    
    // Check if it's time for next measurement
    uint32_t current_time = millis();
    if (current_time - last_measurement_time >= SAMPLING_INTERVAL_MS) {
        performMeasurement();
    }
}

void System::stop() {
    if (!initialized) return;
    
    ESP_LOGI(TAG, "Shutting down system...");
    
    if (sensor) {
        sensor->deinit();
        sensor = nullptr;
    }
    
    i2c_deinit();
    
    initialized = false;
    ESP_LOGI(TAG, "System stopped");
}

uint32_t System::getUptime() const {
    return millis();
}

uint32_t System::getMeasurementCount() const {
    return measurement_count;
}

// ============================================================================
// Private Methods
// ============================================================================

bool System::initPeripherals() {
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    ESP_LOGI(TAG, "Initializing Peripheral Driver Layer...");
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    
    i2c_config_t i2c_config = {
        .sda_pin = I2C_SDA_PIN,
        .scl_pin = I2C_SCL_PIN,
        .frequency_hz = I2C_FREQUENCY_HZ,
        .timeout_ms = I2C_TIMEOUT_MS
    };
    
    i2c_status_t i2c_status = i2c_init(&i2c_config);
    if (i2c_status != I2C_OK) {
        ESP_LOGE(TAG, "I2C initialization failed: %s", 
                 i2c_status_to_string(i2c_status));
        return false;
    }
    
    ESP_LOGI(TAG, "I2C initialized (SDA=%d, SCL=%d, %lu Hz)", 
             I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY_HZ);
    
    // Scan I2C bus
    ESP_LOGI(TAG, "Scanning I2C bus...");
    uint8_t found_devices[10];
    uint8_t device_count = i2c_scan(found_devices, 10);
    
    if (device_count == 0) {
        ESP_LOGW(TAG, "No I2C devices found!");
    } else {
        ESP_LOGI(TAG, "Found %d device(s):", device_count);
        for (uint8_t i = 0; i < device_count; i++) {
            ESP_LOGI(TAG, "  - 0x%02X", found_devices[i]);
        }
    }
    
    return true;
}

bool System::initSensor() {
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    ESP_LOGI(TAG, "Initializing HAL (Sensor Abstraction)...");
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    
    // Get sensor interface
    sensor = sensor_get_interface(SENSOR_MODEL);
    
    if (sensor == NULL) {
        ESP_LOGE(TAG, "Sensor '%s' not found in registry!", SENSOR_MODEL);
        ESP_LOGI(TAG, "Available sensors:");
        
        uint8_t sensor_count;
        const char **sensor_list = sensor_get_available_sensors(&sensor_count);
        for (uint8_t i = 0; i < sensor_count; i++) {
            ESP_LOGI(TAG, "  - %s", sensor_list[i]);
        }
        
        return false;
    }
    
    ESP_LOGI(TAG, "Sensor interface acquired: %s", SENSOR_MODEL);
    
    // Initialize sensor
    ESP_LOGI(TAG, "Initializing sensor hardware...");
    sensor_status_t status = sensor->init();
    
    if (status != SENSOR_OK) {
        ESP_LOGE(TAG, "Sensor initialization failed: %s", 
                 sensor_status_to_string(status));
        ESP_LOGI(TAG, "Troubleshooting:");
        ESP_LOGI(TAG, "  - Check I2C connections (SDA, SCL)");
        ESP_LOGI(TAG, "  - Verify sensor power supply (3.3V)");
        ESP_LOGI(TAG, "  - Check sensor I2C address");
        
        return false;
    }
    
    ESP_LOGI(TAG, "Sensor initialized successfully");
    
    // Print sensor information
    const sensor_info_t *info = sensor->get_info();
    printSensorInfo(info);
    
    // Configure sensor
    sensor_config_t config = {
        .precision = 2,  // High precision
        .temp_offset_celsius = 0.0f,
        .hum_offset_percent = 0.0f,
        .enable_heater = false
    };
    
    status = sensor->configure(&config);
    if (status == SENSOR_OK) {
        ESP_LOGI(TAG, "Sensor configured (HIGH precision mode)");
    } else {
        ESP_LOGW(TAG, "Configuration warning: %s", 
                 sensor_status_to_string(status));
    }
    
    return true;
}

void System::printSensorInfo(const sensor_info_t *info) {
    ESP_LOGI(TAG, "Sensor Information:");
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    ESP_LOGI(TAG, "  Model:         %s", info->name);
    ESP_LOGI(TAG, "  Manufacturer:  %s", info->manufacturer);
    ESP_LOGI(TAG, "  Interface:     %s", info->interface_type);
    ESP_LOGI(TAG, "  Temp Range:    %.1f°C to %.1f°C", 
             info->temp_min, info->temp_max);
    ESP_LOGI(TAG, "  Humidity Range: %.0f%% to %.0f%%", 
             info->hum_min, info->hum_max);
    ESP_LOGI(TAG, "  Temp Accuracy:  ±%.1f°C", info->temp_accuracy);
    ESP_LOGI(TAG, "  Hum Accuracy:   ±%.1f%%", info->hum_accuracy);
    ESP_LOGI(TAG, "  Meas. Time:     %u ms", info->measurement_time_ms);
    ESP_LOGI(TAG, "  Active Current: %u µA", info->power_active_ua);
    ESP_LOGI(TAG, "  Sleep Current:  %u µA", info->power_sleep_ua);
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
}

void System::printSensorData(const sensor_data_t *data) {
    ESP_LOGI(TAG, "┌─────────────────────────────────────────┐");
    ESP_LOGI(TAG, "│ Measurement #%-5lu                    │", measurement_count);
    ESP_LOGI(TAG, "├─────────────────────────────────────────┤");
    ESP_LOGI(TAG, "│ Temperature:  %6.2f °C              │", 
             data->temperature_celsius);
    ESP_LOGI(TAG, "│ Humidity:     %6.1f %%               │", 
             data->humidity_percent);
    ESP_LOGI(TAG, "│ Timestamp:    %10lu ms           │", 
             data->timestamp);
    ESP_LOGI(TAG, "│ Temp Valid:   %s                       │", 
             (data->quality_flags & 0x80) ? "✓ Yes" : "✗ No ");
    ESP_LOGI(TAG, "│ Hum Valid:    %s                       │", 
             (data->quality_flags & 0x40) ? "✓ Yes" : "✗ No ");
    ESP_LOGI(TAG, "└─────────────────────────────────────────┘");
    
    // Environmental assessment for basil cultivation
    ESP_LOGI(TAG, "Basil Growth Conditions Assessment:");
    ESP_LOGI(TAG, "───────────────────────────────────");
    
    // Optimal basil temperature: 18-25°C
    if (data->temperature_celsius < 15.0f) {
        ESP_LOGW(TAG, "  Temperature TOO LOW (%.1f°C) - Risk of growth stunting", 
                 data->temperature_celsius);
    } else if (data->temperature_celsius < 18.0f) {
        ESP_LOGW(TAG, "  Temperature BELOW OPTIMAL (%.1f°C) - Slower growth", 
                 data->temperature_celsius);
    } else if (data->temperature_celsius <= 25.0f) {
        ESP_LOGI(TAG, "  Temperature OPTIMAL (%.1f°C) - Ideal for basil", 
                 data->temperature_celsius);
    } else if (data->temperature_celsius <= 30.0f) {
        ESP_LOGW(TAG, "  Temperature ABOVE OPTIMAL (%.1f°C) - Monitor stress", 
                 data->temperature_celsius);
    } else {
        ESP_LOGE(TAG, "  Temperature TOO HIGH (%.1f°C) - Risk of wilting!", 
                 data->temperature_celsius);
    }
    
    // Optimal basil humidity: 60-70%
    if (data->humidity_percent < 40.0f) {
        ESP_LOGW(TAG, "  Humidity TOO LOW (%.1f%%) - Risk of leaf drying", 
                 data->humidity_percent);
    } else if (data->humidity_percent < 60.0f) {
        ESP_LOGW(TAG, "  Humidity BELOW OPTIMAL (%.1f%%) - Increase misting", 
                 data->humidity_percent);
    } else if (data->humidity_percent <= 70.0f) {
        ESP_LOGI(TAG, "  Humidity OPTIMAL (%.1f%%) - Ideal for basil", 
                 data->humidity_percent);
    } else if (data->humidity_percent <= 80.0f) {
        ESP_LOGW(TAG, "  Humidity ABOVE OPTIMAL (%.1f%%) - Monitor mold risk", 
                 data->humidity_percent);
    } else {
        ESP_LOGE(TAG, "  Humidity TOO HIGH (%.1f%%) - Risk of fungal disease!", 
                 data->humidity_percent);
    }
    
    ESP_LOGI(TAG, "");  // Blank line
}

void System::performMeasurement() {
    measurement_count++;
    last_measurement_time = millis();
    
    ESP_LOGI(TAG, "[%lu] Triggering measurement...", millis());
    
    // Trigger measurement
    sensor_status_t status = sensor->trigger_measurement();
    if (status != SENSOR_OK) {
        ESP_LOGE(TAG, "Trigger failed: %s", sensor_status_to_string(status));
        return;
    }
    
    // Wait for measurement to complete
    const sensor_info_t *info = sensor->get_info();
    delay(info->measurement_time_ms);
    
    // Read data
    sensor_data_t data;
    status = sensor->read(&data);
    
    if (status != SENSOR_OK) {
        ESP_LOGE(TAG, "Read failed: %s", sensor_status_to_string(status));
        return;
    }
    
    // Validate data
    if (!sensor_data_is_valid(&data)) {
        ESP_LOGW(TAG, "Warning: Invalid sensor data");
        return;
    }
    
    // Print data
    printSensorData(&data);
}

