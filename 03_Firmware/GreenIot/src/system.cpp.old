/**
 * @file system.cpp
 * @brief System Manager Implementation
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 * @version 3.0 (BLE Mesh Integration)
 */

#include "system.h"
#include <Arduino.h>
#include "Drivers/i2c_driver.h"
#include "HAL/Wireless/ble_mesh_config.h"
#include "esp_log.h"
#include <string.h>
#include <math.h>

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

// Sampling interval - 5 minutes (matches BLE Mesh publish interval)
#define SAMPLING_INTERVAL_MS BLE_MESH_PUBLISH_INTERVAL_MS

// BLE Mesh Publishing interval - 5 minutes
#define MESH_PUBLISH_INTERVAL_MS BLE_MESH_PUBLISH_INTERVAL_MS

// Enable BLE Mesh (set to false to disable for testing)
#define ENABLE_BLE_MESH true

// ============================================================================
// Public Methods
// ============================================================================

System::System() 
    : sensor(nullptr)
    , ble_mesh_enabled(ENABLE_BLE_MESH)
    , has_last_sensor_data(false)
    , measurement_count(0)
    , last_measurement_time(0)
    , last_publish_time(0)
    , initialized(false)
{
    memset(&last_sensor_data, 0, sizeof(sensor_data_t));
}

System::~System() {
    stop();
}

bool System::init() {
    ESP_LOGI(TAG, "\n╔══════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║   GreenIoT Vertical Farming Project      ║");
    ESP_LOGI(TAG, "║   Environmental Monitoring Sensor Node   ║");
    ESP_LOGI(TAG, "║   BLE Mesh Sensor Server                 ║");
    ESP_LOGI(TAG, "╚══════════════════════════════════════════╝\n");
    
    ESP_LOGI(TAG, "Architecture Layers:");
    ESP_LOGI(TAG, "  [5] Application Layer    ← System Manager");
    ESP_LOGI(TAG, "  [4] Service Layer        ← BLE Mesh Communication");
    ESP_LOGI(TAG, "  [3] HAL Layer            ← sensor_interface + ble_mesh_interface");
    ESP_LOGI(TAG, "  [2] Peripheral Driver    ← i2c_driver");
    ESP_LOGI(TAG, "  [1] Hardware Layer       ← ESP32-C3 / Wire.h / BLE 5.0\n");
    
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
    
    // Initialize BLE Mesh
    if (ble_mesh_enabled) {
        if (!initBLEMesh()) {
            ESP_LOGW(TAG, "BLE Mesh initialization failed - continuing without mesh");
            ble_mesh_enabled = false;
        }
    } else {
        ESP_LOGI(TAG, "BLE Mesh disabled - operating in standalone mode");
    }
    
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    ESP_LOGI(TAG, "Sampling Interval: %lu seconds (%.1f minutes)", 
             SAMPLING_INTERVAL_MS / 1000, SAMPLING_INTERVAL_MS / 60000.0f);
    if (ble_mesh_enabled) {
        ESP_LOGI(TAG, "Publishing Interval: %lu seconds (%.1f minutes)",
                 MESH_PUBLISH_INTERVAL_MS / 1000, MESH_PUBLISH_INTERVAL_MS / 60000.0f);
    }
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

bool System::isMeshProvisioned() const {
    if (!ble_mesh_enabled) {
        return false;
    }
    return ble_mesh_is_provisioned();
}

ble_mesh_status_t System::getMeshStatus() const {
    ble_mesh_status_t status = {0};
    if (ble_mesh_enabled) {
        ble_mesh_get_status(&status);
    }
    return status;
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

bool System::initBLEMesh() {
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    ESP_LOGI(TAG, "Initializing BLE Mesh Sensor Server...");
    ESP_LOGI(TAG, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    
    // Get MAC address for device name
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    
    // Configure BLE Mesh
    ble_mesh_config_t config = {0};
    
    // Generate device name
    snprintf(config.device_name, BLE_MESH_DEV_NAME_MAX_LEN, 
             "GreenIoT-%02X%02X%02X", mac[3], mac[4], mac[5]);
    
    // Set company/product IDs
    config.company_id = BLE_MESH_COMPANY_ID_ESPRESSIF;
    config.product_id = 0x0001;  // GreenIoT Sensor Node
    config.version_id = 0x0001;  // Version 1.0
    
    // Enable provisioning methods
    config.enable_pb_adv = true;   // PB-ADV (Advertising Bearer)
    config.enable_pb_gatt = true;  // PB-GATT (GATT Bearer)
    
    // Enable Low Power Node
    config.enable_lpn = true;
    config.lpn_poll_interval_ms = BLE_MESH_LPN_POLL_INTERVAL_MS;
    
    // Set features
    config.features = 0;
    config.features |= BLE_MESH_FEATURE_LOW_POWER;  // LPN enabled
    config.features |= BLE_MESH_FEATURE_PROXY;      // GATT Proxy for provisioning
    
    // Initialize BLE Mesh
    ble_mesh_err_t status = ble_mesh_init(&config);
    
    if (status != BLE_MESH_OK) {
        ESP_LOGE(TAG, "BLE Mesh initialization failed: %s", 
                 ble_mesh_status_to_string(status));
        return false;
    }
    
    // Enable provisioning beacon
    status = ble_mesh_enable_provisioning();
    if (status != BLE_MESH_OK) {
        ESP_LOGW(TAG, "Failed to enable provisioning: %s", 
                 ble_mesh_status_to_string(status));
    }
    
    ESP_LOGI(TAG, "✓ BLE Mesh Sensor Server initialized");
    ESP_LOGI(TAG, "Device Name: %s", config.device_name);
    ESP_LOGI(TAG, "");
    
    return true;
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
    
    // Check if we should publish to mesh
    if (ble_mesh_enabled) {
        uint32_t current_time = millis();
        bool time_to_publish = (current_time - last_publish_time) >= MESH_PUBLISH_INTERVAL_MS;
        bool significant_change = shouldPublishImmediately(&data);
        
        if (time_to_publish || significant_change) {
            if (significant_change) {
                ESP_LOGI(TAG, "▶ Significant environmental change detected - publishing immediately!");
            }
            publishToMesh(&data);
            last_publish_time = current_time;
        }
    }
    
    // Store last sensor data for comparison
    memcpy(&last_sensor_data, &data, sizeof(sensor_data_t));
    has_last_sensor_data = true;
}

void System::publishToMesh(const sensor_data_t *data) {
    if (!ble_mesh_enabled) {
        return;
    }
    
    // Prepare mesh sensor data
    mesh_sensor_data_t mesh_data;
    mesh_data.temperature_celsius = data->temperature_celsius;
    mesh_data.humidity_percent = data->humidity_percent;
    mesh_data.battery_level = 95;  // TODO: Implement battery monitoring
    mesh_data.timestamp = data->timestamp;
    
    // Publish to mesh
    ble_mesh_err_t status = ble_mesh_publish_sensor_data(&mesh_data);
    
    if (status != BLE_MESH_OK) {
        ESP_LOGE(TAG, "Failed to publish sensor data: %s", 
                 ble_mesh_status_to_string(status));
    }
}

bool System::shouldPublishImmediately(const sensor_data_t *data) {
    if (!has_last_sensor_data) {
        return false;  // No previous data to compare
    }
    
    // Check temperature change
    float temp_change = fabsf(data->temperature_celsius - last_sensor_data.temperature_celsius);
    if (temp_change >= BLE_MESH_TEMP_CHANGE_THRESHOLD) {
        ESP_LOGW(TAG, "Temperature changed by %.2f°C (threshold: %.1f°C)", 
                 temp_change, BLE_MESH_TEMP_CHANGE_THRESHOLD);
        return true;
    }
    
    // Check humidity change
    float hum_change = fabsf(data->humidity_percent - last_sensor_data.humidity_percent);
    if (hum_change >= BLE_MESH_HUM_CHANGE_THRESHOLD) {
        ESP_LOGW(TAG, "Humidity changed by %.1f%% (threshold: %.1f%%)", 
                 hum_change, BLE_MESH_HUM_CHANGE_THRESHOLD);
        return true;
    }
    
    // Check critical thresholds for basil
    if (data->temperature_celsius < BASIL_TEMP_MIN_CRITICAL || 
        data->temperature_celsius > BASIL_TEMP_MAX_CRITICAL) {
        ESP_LOGE(TAG, "CRITICAL: Temperature outside safe range for basil!");
        return true;
    }
    
    if (data->humidity_percent < BASIL_HUM_MIN_CRITICAL || 
        data->humidity_percent > BASIL_HUM_MAX_CRITICAL) {
        ESP_LOGE(TAG, "CRITICAL: Humidity outside safe range for basil!");
        return true;
    }
    
    return false;  // No significant change
}

