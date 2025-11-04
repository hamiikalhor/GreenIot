/**
 * @file mock_ble_mesh.cpp
 * @brief Mock Implementation of BLE Mesh Interface for Unit Testing
 * 
 * This mock provides simulated BLE Mesh functionality for testing without
 * requiring actual BLE hardware. All functions return success and maintain
 * internal state for verification.
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 */

#include "HAL/Wireless/ble_mesh_interface.h"
#include <string.h>

#ifdef NATIVE_BUILD
#include "Arduino_stub.h"
#else
#include <Arduino.h>
#endif

// ============================================================================
// Mock Internal State
// ============================================================================

static bool g_mock_initialized = false;
static bool g_mock_provisioned = false;
static bool g_mock_provisioning_enabled = false;
static ble_mesh_config_t g_mock_config;
static ble_mesh_status_t g_mock_status;
static mesh_sensor_data_t g_mock_last_published_data;
static uint32_t g_mock_publish_count = 0;

// ============================================================================
// Mock BLE Mesh Functions
// ============================================================================

ble_mesh_err_t ble_mesh_init(const ble_mesh_config_t *config) {
    if (config == NULL) {
        return BLE_MESH_ERROR_INVALID_PARAM;
    }
    
    if (g_mock_initialized) {
        return BLE_MESH_ERROR_ALREADY_INITIALIZED;
    }
    
    // Copy configuration
    memcpy(&g_mock_config, config, sizeof(ble_mesh_config_t));
    
    // Initialize mock status
    g_mock_status.initialized = true;
    g_mock_status.provisioned = false;
    g_mock_status.unicast_addr = 0x0000;
    g_mock_status.net_idx = 0x0000;
    g_mock_status.app_idx = 0x0000;
    g_mock_status.prov_state = BLE_MESH_PROV_STATE_IDLE;
    g_mock_status.features = config->features;
    g_mock_status.uptime_ms = millis();
    
    g_mock_initialized = true;
    g_mock_provisioned = false;
    g_mock_provisioning_enabled = false;
    g_mock_publish_count = 0;
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_deinit(void) {
    if (!g_mock_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    g_mock_initialized = false;
    g_mock_provisioned = false;
    g_mock_provisioning_enabled = false;
    g_mock_status.initialized = false;
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_enable_provisioning(void) {
    if (!g_mock_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    if (g_mock_provisioned) {
        return BLE_MESH_ERROR_ALREADY_PROVISIONED;
    }
    
    g_mock_provisioning_enabled = true;
    g_mock_status.prov_state = BLE_MESH_PROV_STATE_LINK_OPEN;
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_disable_provisioning(void) {
    if (!g_mock_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    g_mock_provisioning_enabled = false;
    g_mock_status.prov_state = BLE_MESH_PROV_STATE_IDLE;
    
    return BLE_MESH_OK;
}

bool ble_mesh_is_provisioned(void) {
    return g_mock_provisioned;
}

ble_mesh_err_t ble_mesh_publish_sensor_data(const mesh_sensor_data_t *data) {
    if (!g_mock_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    if (data == NULL) {
        return BLE_MESH_ERROR_INVALID_PARAM;
    }
    
    // Store last published data for verification
    memcpy(&g_mock_last_published_data, data, sizeof(mesh_sensor_data_t));
    g_mock_publish_count++;
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_get_status(ble_mesh_status_t *status) {
    if (!g_mock_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    if (status == NULL) {
        return BLE_MESH_ERROR_INVALID_PARAM;
    }
    
    // Update uptime
    g_mock_status.uptime_ms = millis() - g_mock_status.uptime_ms;
    
    memcpy(status, &g_mock_status, sizeof(ble_mesh_status_t));
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_register_prov_callback(ble_mesh_prov_callback_t callback) {
    if (!g_mock_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    // In mock, we just accept the callback but don't use it
    (void)callback;
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_register_sensor_callback(ble_mesh_sensor_callback_t callback) {
    if (!g_mock_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    // In mock, we just accept the callback but don't use it
    (void)callback;
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_factory_reset(void) {
    if (!g_mock_initialized) {
        return BLE_MESH_ERROR_NOT_INITIALIZED;
    }
    
    g_mock_provisioned = false;
    g_mock_status.provisioned = false;
    g_mock_status.unicast_addr = 0x0000;
    g_mock_status.net_idx = 0x0000;
    g_mock_status.app_idx = 0x0000;
    g_mock_status.prov_state = BLE_MESH_PROV_STATE_IDLE;
    
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
// Mock Helper Functions (for test verification)
// ============================================================================

/**
 * @brief Get the number of times sensor data was published (for test verification)
 */
uint32_t mock_ble_mesh_get_publish_count(void) {
    return g_mock_publish_count;
}

/**
 * @brief Get the last published sensor data (for test verification)
 */
const mesh_sensor_data_t* mock_ble_mesh_get_last_published_data(void) {
    return &g_mock_last_published_data;
}

/**
 * @brief Reset mock state (for test cleanup)
 */
void mock_ble_mesh_reset(void) {
    g_mock_initialized = false;
    g_mock_provisioned = false;
    g_mock_provisioning_enabled = false;
    g_mock_publish_count = 0;
    memset(&g_mock_config, 0, sizeof(ble_mesh_config_t));
    memset(&g_mock_status, 0, sizeof(ble_mesh_status_t));
    memset(&g_mock_last_published_data, 0, sizeof(mesh_sensor_data_t));
}

/**
 * @brief Simulate successful provisioning (for test scenarios)
 */
void mock_ble_mesh_simulate_provisioning(uint16_t unicast_addr, uint16_t net_idx, uint16_t app_idx) {
    if (g_mock_initialized) {
        g_mock_provisioned = true;
        g_mock_status.provisioned = true;
        g_mock_status.unicast_addr = unicast_addr;
        g_mock_status.net_idx = net_idx;
        g_mock_status.app_idx = app_idx;
        g_mock_status.prov_state = BLE_MESH_PROV_STATE_SUCCESS;
    }
}

/**
 * @brief Check if provisioning is enabled (for test verification)
 */
bool mock_ble_mesh_is_provisioning_enabled(void) {
    return g_mock_provisioning_enabled;
}

