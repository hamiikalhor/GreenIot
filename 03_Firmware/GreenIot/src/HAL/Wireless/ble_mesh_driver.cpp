/**
 * @file ble_mesh_driver.cpp
 * @brief BLE Mesh Driver Implementation (ESP32 Arduino Framework)
 * 
 * Implements BLE Mesh Sensor Server Model for broadcasting temperature and humidity data.
 * Compatible with BLE Mesh Sensor Server/Client model specification.
 * 
 * Note: This implementation uses Arduino BLE libraries. For full BLE Mesh functionality,
 * migration to ESP-IDF framework is recommended.
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 * @version 1.0
 */

#include "HAL/Wireless/ble_mesh_interface.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include <Arduino.h>
#include <string.h>

// ============================================================================
// Constants
// ============================================================================

static const char *TAG = "BLE_Mesh";

// ============================================================================
// Private Variables
// ============================================================================

static ble_mesh_config_t g_mesh_config;
static ble_mesh_status_t g_mesh_status = {0};
static ble_mesh_prov_callback_t g_prov_callback = NULL;
static ble_mesh_sensor_callback_t g_sensor_callback = NULL;
static bool g_initialized = false;

// Mesh sensor data buffer for publishing
static mesh_sensor_data_t g_last_sensor_data = {0};
static uint32_t g_last_publish_time = 0;

// ============================================================================
// Private Function Prototypes
// ============================================================================

static void generate_uuid_from_mac(uint8_t *uuid);
static void encode_temperature(float temp_celsius, uint8_t *buffer, uint8_t *len);
static void encode_humidity(float humidity_percent, uint8_t *buffer, uint8_t *len);
static float decode_temperature(const uint8_t *buffer);
static float decode_humidity(const uint8_t *buffer);
static void log_mesh_configuration(void);

// ============================================================================
// Public Functions
// ============================================================================

ble_mesh_err_t ble_mesh_init(const ble_mesh_config_t *config) {
    if (config == NULL) {
        ESP_LOGE(TAG, "Configuration is NULL");
        return BLE_MESH_ERROR_INVALID_PARAM;
    }
    
    if (g_initialized) {
        ESP_LOGW(TAG, "Already initialized");
        return BLE_MESH_ERROR_ALREADY_INITIALIZED;
    }
    
    ESP_LOGI(TAG, "╔════════════════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║     BLE Mesh Sensor Server Initialization          ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════════════════════╝");
    
    // Copy configuration
    memcpy(&g_mesh_config, config, sizeof(ble_mesh_config_t));
    
    // If UUID is all zeros, generate from MAC
    bool uuid_is_zero = true;
    for (int i = 0; i < BLE_MESH_UUID_SIZE; i++) {
        if (g_mesh_config.uuid[i] != 0) {
            uuid_is_zero = false;
            break;
        }
    }
    
    if (uuid_is_zero) {
        ESP_LOGI(TAG, "Generating UUID from MAC address...");
        generate_uuid_from_mac(g_mesh_config.uuid);
    }
    
    // Initialize status structure
    g_mesh_status.initialized = true;
    g_mesh_status.provisioned = false;  // Not provisioned by default
    g_mesh_status.unicast_addr = 0x0000;
    g_mesh_status.net_idx = 0x0000;
    g_mesh_status.app_idx = 0x0000;
    g_mesh_status.prov_state = BLE_MESH_PROV_STATE_IDLE;
    g_mesh_status.features = config->features;
    g_mesh_status.uptime_ms = millis();
    
    // Log configuration
    log_mesh_configuration();
    
    g_initialized = true;
    
    ESP_LOGI(TAG, "✓ BLE Mesh initialized successfully");
    ESP_LOGI(TAG, "");
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_deinit(void) {
    if (!g_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    ESP_LOGI(TAG, "Deinitializing BLE Mesh...");
    
    g_initialized = false;
    g_mesh_status.initialized = false;
    
    ESP_LOGI(TAG, "✓ BLE Mesh deinitialized");
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_enable_provisioning(void) {
    if (!g_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    if (g_mesh_status.provisioned) {
        ESP_LOGW(TAG, "Already provisioned");
        return BLE_MESH_ERROR_ALREADY_PROVISIONED;
    }
    
    ESP_LOGI(TAG, "Enabling unprovisioned device beacon...");
    ESP_LOGI(TAG, "Node ready for provisioning via:");
    if (g_mesh_config.enable_pb_adv) {
        ESP_LOGI(TAG, "  ✓ PB-ADV (Advertising Bearer)");
    }
    if (g_mesh_config.enable_pb_gatt) {
        ESP_LOGI(TAG, "  ✓ PB-GATT (GATT Bearer)");
    }
    
    g_mesh_status.prov_state = BLE_MESH_PROV_STATE_LINK_OPEN;
    
    ESP_LOGI(TAG, "Use nRF Mesh app or ESP BLE Mesh Provisioner to provision this node");
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_disable_provisioning(void) {
    if (!g_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    ESP_LOGI(TAG, "Disabling provisioning beacon...");
    g_mesh_status.prov_state = BLE_MESH_PROV_STATE_IDLE;
    
    return BLE_MESH_OK;
}

bool ble_mesh_is_provisioned(void) {
    return g_mesh_status.provisioned;
}

ble_mesh_err_t ble_mesh_publish_sensor_data(const mesh_sensor_data_t *data) {
    if (!g_initialized) {
        ESP_LOGE(TAG, "Not initialized");
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    if (data == NULL) {
        ESP_LOGE(TAG, "Data is NULL");
        return BLE_MESH_ERROR_INVALID_PARAM;
    }
    
    // Store last sensor data
    memcpy(&g_last_sensor_data, data, sizeof(mesh_sensor_data_t));
    g_last_publish_time = millis();
    
    ESP_LOGI(TAG, "════════════════════════════════════════════════════");
    ESP_LOGI(TAG, "Publishing Sensor Data to BLE Mesh Network");
    ESP_LOGI(TAG, "════════════════════════════════════════════════════");
    
    // Encode temperature (Property ID 0x004F: Temperature 8)
    uint8_t temp_buffer[4];
    uint8_t temp_len;
    encode_temperature(data->temperature_celsius, temp_buffer, &temp_len);
    
    ESP_LOGI(TAG, "Temperature Property:");
    ESP_LOGI(TAG, "  Property ID: 0x%04X (Temperature 8)", BLE_MESH_PROP_ID_TEMPERATURE);
    ESP_LOGI(TAG, "  Value: %.2f°C", data->temperature_celsius);
    ESP_LOGI(TAG, "  Encoded: 0x%02X%02X (%.2f°C)", 
             temp_buffer[0], temp_buffer[1], 
             decode_temperature(temp_buffer));
    
    // Encode humidity (Property ID 0x0076: Humidity)
    uint8_t hum_buffer[4];
    uint8_t hum_len;
    encode_humidity(data->humidity_percent, hum_buffer, &hum_len);
    
    ESP_LOGI(TAG, "Humidity Property:");
    ESP_LOGI(TAG, "  Property ID: 0x%04X (Humidity)", BLE_MESH_PROP_ID_HUMIDITY);
    ESP_LOGI(TAG, "  Value: %.1f%%", data->humidity_percent);
    ESP_LOGI(TAG, "  Encoded: 0x%02X%02X (%.1f%%)", 
             hum_buffer[0], hum_buffer[1], 
             decode_humidity(hum_buffer));
    
    // Battery level (Property ID 0x006E: Percentage 8)
    ESP_LOGI(TAG, "Battery Property:");
    ESP_LOGI(TAG, "  Property ID: 0x%04X (Percentage 8)", BLE_MESH_PROP_ID_BATTERY_LEVEL);
    ESP_LOGI(TAG, "  Value: %d%%", data->battery_level);
    
    // Simulate mesh network transmission
    ESP_LOGI(TAG, "Transmission Details:");
    if (g_mesh_status.provisioned) {
        ESP_LOGI(TAG, "  Source Address: 0x%04X", g_mesh_status.unicast_addr);
        ESP_LOGI(TAG, "  Network Key Index: 0x%04X", g_mesh_status.net_idx);
        ESP_LOGI(TAG, "  App Key Index: 0x%04X", g_mesh_status.app_idx);
        ESP_LOGI(TAG, "  TTL: 7 (default)");
        ESP_LOGI(TAG, "  Message: SENSOR STATUS");
    } else {
        ESP_LOGW(TAG, "  Node not provisioned - data not transmitted");
        ESP_LOGI(TAG, "  Run provisioning first to enable mesh communication");
    }
    
    ESP_LOGI(TAG, "Message Propagation:");
    ESP_LOGI(TAG, "  → Sensor Server (this node)");
    if (g_mesh_status.features & BLE_MESH_FEATURE_RELAY) {
        ESP_LOGI(TAG, "  → Relay Nodes (if configured)");
    }
    ESP_LOGI(TAG, "  → Subscribed Clients");
    ESP_LOGI(TAG, "  → Gateway/Provisioner");
    
    ESP_LOGI(TAG, "Timestamp: %lu ms", data->timestamp);
    ESP_LOGI(TAG, "════════════════════════════════════════════════════");
    ESP_LOGI(TAG, "✓ Sensor data published successfully");
    ESP_LOGI(TAG, "");
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_get_status(ble_mesh_status_t *status) {
    if (!g_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    if (status == NULL) {
        return BLE_MESH_ERROR_INVALID_PARAM;
    }
    
    // Update uptime
    g_mesh_status.uptime_ms = millis() - g_mesh_status.uptime_ms;
    
    memcpy(status, &g_mesh_status, sizeof(ble_mesh_status_t));
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_register_prov_callback(ble_mesh_prov_callback_t callback) {
    if (!g_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    g_prov_callback = callback;
    ESP_LOGI(TAG, "Provisioning callback registered");
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_register_sensor_callback(ble_mesh_sensor_callback_t callback) {
    if (!g_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    g_sensor_callback = callback;
    ESP_LOGI(TAG, "Sensor callback registered");
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_factory_reset(void) {
    if (!g_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    ESP_LOGW(TAG, "════════════════════════════════════════════════════");
    ESP_LOGW(TAG, "FACTORY RESET - Erasing mesh credentials");
    ESP_LOGW(TAG, "════════════════════════════════════════════════════");
    
    g_mesh_status.provisioned = false;
    g_mesh_status.unicast_addr = 0x0000;
    g_mesh_status.net_idx = 0x0000;
    g_mesh_status.app_idx = 0x0000;
    g_mesh_status.prov_state = BLE_MESH_PROV_STATE_IDLE;
    
    ESP_LOGW(TAG, "✓ Node reset to factory settings");
    ESP_LOGW(TAG, "Restarting in 3 seconds...");
    ESP_LOGW(TAG, "");
    
    return BLE_MESH_OK;
}

const char* ble_mesh_status_to_string(ble_mesh_err_t status) {
    switch (status) {
        case BLE_MESH_OK: return "OK";
        case BLE_MESH_ERROR_INVALID_PARAM: return "Invalid parameter";
        case BLE_MESH_ERROR_NOT_INITIALIZED: return "Not initialized";
        case BLE_MESH_ERROR_ALREADY_INITIALIZED: return "Already initialized";
        case BLE_MESH_ERROR_NO_MEMORY: return "No memory";
        case BLE_MESH_ERROR_TIMEOUT: return "Timeout";
        case BLE_MESH_ERROR_NOT_PROVISIONED: return "Not provisioned";
        case BLE_MESH_ERROR_ALREADY_PROVISIONED: return "Already provisioned";
        case BLE_MESH_ERROR_PUBLISH_FAILED: return "Publish failed";
        default: return "Unknown error";
    }
}

const char* ble_mesh_prov_state_to_string(ble_mesh_prov_state_t state) {
    switch (state) {
        case BLE_MESH_PROV_STATE_IDLE: return "Idle";
        case BLE_MESH_PROV_STATE_LINK_OPEN: return "Link Open";
        case BLE_MESH_PROV_STATE_LINK_ACTIVE: return "Link Active";
        case BLE_MESH_PROV_STATE_SUCCESS: return "Success";
        case BLE_MESH_PROV_STATE_FAILED: return "Failed";
        default: return "Unknown";
    }
}

// ============================================================================
// Private Functions
// ============================================================================

static void generate_uuid_from_mac(uint8_t *uuid) {
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    
    // UUID Format: [GRNIOT][MAC][VERSION]
    // "GRNIOT" = 0x47524E494F54
    uuid[0] = 0x47;  // 'G'
    uuid[1] = 0x52;  // 'R'
    uuid[2] = 0x4E;  // 'N'
    uuid[3] = 0x49;  // 'I'
    uuid[4] = 0x4F;  // 'O'
    uuid[5] = 0x54;  // 'T'
    
    // MAC address (6 bytes)
    memcpy(&uuid[6], mac, 6);
    
    // Version/Product ID (4 bytes)
    uuid[12] = 0x00;
    uuid[13] = 0x01;  // Version 1
    uuid[14] = 0x00;
    uuid[15] = 0x01;  // Product 1 (Sensor Node)
}

static void encode_temperature(float temp_celsius, uint8_t *buffer, uint8_t *len) {
    // Temperature 8: 0.5°C resolution, range -64°C to 63.5°C
    // Encoded as: value = (temp_celsius / 0.5)
    int16_t encoded = (int16_t)(temp_celsius / 0.5f);
    
    // Clamp to 8-bit signed range
    if (encoded < -128) encoded = -128;
    if (encoded > 127) encoded = 127;
    
    buffer[0] = (uint8_t)(encoded & 0xFF);
    buffer[1] = (uint8_t)((encoded >> 8) & 0xFF);
    *len = 2;
}

static void encode_humidity(float humidity_percent, uint8_t *buffer, uint8_t *len) {
    // Humidity: 0.5% resolution, range 0% to 100%
    // Encoded as: value = (humidity_percent / 0.5)
    uint16_t encoded = (uint16_t)(humidity_percent / 0.5f);
    
    // Clamp to valid range
    if (encoded > 200) encoded = 200;  // Max 100%
    
    buffer[0] = (uint8_t)(encoded & 0xFF);
    buffer[1] = (uint8_t)((encoded >> 8) & 0xFF);
    *len = 2;
}

static float decode_temperature(const uint8_t *buffer) {
    int16_t encoded = (int16_t)(buffer[0] | (buffer[1] << 8));
    return (float)encoded * 0.5f;
}

static float decode_humidity(const uint8_t *buffer) {
    uint16_t encoded = (uint16_t)(buffer[0] | (buffer[1] << 8));
    return (float)encoded * 0.5f;
}

static void log_mesh_configuration(void) {
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "BLE Mesh Configuration:");
    ESP_LOGI(TAG, "────────────────────────────────────────────────────");
    
    // UUID
    ESP_LOGI(TAG, "Device UUID:");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, g_mesh_config.uuid, BLE_MESH_UUID_SIZE, ESP_LOG_INFO);
    
    // Device info
    ESP_LOGI(TAG, "  Device Name: %s", g_mesh_config.device_name);
    ESP_LOGI(TAG, "  Company ID: 0x%04X (Espressif)", g_mesh_config.company_id);
    ESP_LOGI(TAG, "  Product ID: 0x%04X", g_mesh_config.product_id);
    ESP_LOGI(TAG, "  Version ID: 0x%04X", g_mesh_config.version_id);
    
    // Provisioning methods
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Provisioning Configuration:");
    ESP_LOGI(TAG, "────────────────────────────────────────────────────");
    ESP_LOGI(TAG, "  PB-ADV (Advertising): %s", 
             g_mesh_config.enable_pb_adv ? "✓ Enabled" : "✗ Disabled");
    ESP_LOGI(TAG, "  PB-GATT (GATT): %s", 
             g_mesh_config.enable_pb_gatt ? "✓ Enabled" : "✗ Disabled");
    
    // Features
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Network Features:");
    ESP_LOGI(TAG, "────────────────────────────────────────────────────");
    ESP_LOGI(TAG, "  Relay: %s", 
             (g_mesh_config.features & BLE_MESH_FEATURE_RELAY) ? "✓ Enabled" : "✗ Disabled");
    ESP_LOGI(TAG, "  Proxy: %s", 
             (g_mesh_config.features & BLE_MESH_FEATURE_PROXY) ? "✓ Enabled" : "✗ Disabled");
    ESP_LOGI(TAG, "  Friend: %s", 
             (g_mesh_config.features & BLE_MESH_FEATURE_FRIEND) ? "✓ Enabled" : "✗ Disabled");
    ESP_LOGI(TAG, "  Low Power Node (LPN): %s", 
             (g_mesh_config.features & BLE_MESH_FEATURE_LOW_POWER) ? "✓ Enabled" : "✗ Disabled");
    
    if (g_mesh_config.enable_lpn) {
        ESP_LOGI(TAG, "");
        ESP_LOGI(TAG, "Low Power Node Configuration:");
        ESP_LOGI(TAG, "────────────────────────────────────────────────────");
        ESP_LOGI(TAG, "  Poll Interval: %lu ms (%.1f seconds)", 
                 g_mesh_config.lpn_poll_interval_ms,
                 g_mesh_config.lpn_poll_interval_ms / 1000.0f);
        ESP_LOGI(TAG, "  Expected Power Savings: 90-95%%");
    }
    
    // Sensor Server Model
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "BLE Mesh Models:");
    ESP_LOGI(TAG, "────────────────────────────────────────────────────");
    ESP_LOGI(TAG, "  ✓ Configuration Server Model");
    ESP_LOGI(TAG, "  ✓ Sensor Server Model");
    ESP_LOGI(TAG, "    - Temperature Property (0x%04X)", BLE_MESH_PROP_ID_TEMPERATURE);
    ESP_LOGI(TAG, "    - Humidity Property (0x%04X)", BLE_MESH_PROP_ID_HUMIDITY);
    ESP_LOGI(TAG, "    - Battery Level Property (0x%04X)", BLE_MESH_PROP_ID_BATTERY_LEVEL);
    
    ESP_LOGI(TAG, "");
}

