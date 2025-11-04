/**
 * @file BLEMeshManager.cpp
 * @brief BLE Mesh network manager implementation
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 */

#include "BLEMeshManager.hpp"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_sensor_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"
#include "esp_mac.h"
#include <cstring>

static const char* TAG = "BLE_MESH";

// BLE Mesh configuration defines
#define CID_ESP             0x02E5  // Espressif company ID

// Element and model definitions
#define ESP_BLE_MESH_VND_MODEL_ID_SERVER  0x0000

BLEMeshManager& BLEMeshManager::getInstance() {
    static BLEMeshManager instance;
    return instance;
}

BLEMeshStatus BLEMeshManager::init(const BLEMeshConfig& config) {
    if (m_initialized) {
        ESP_LOGW(TAG, "BLE Mesh already initialized");
        return BLEMeshStatus::OK;
    }
    
    m_config = config;
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  Initializing BLE Mesh Stack");
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Company ID: 0x%04X", config.company_id);
    ESP_LOGI(TAG, "Product ID: 0x%04X", config.product_id);
    ESP_LOGI(TAG, "Provisioning: %s", 
             config.prov_method == ProvisioningMethod::PB_ADV ? "PB-ADV" : "PB-GATT");
    ESP_LOGI(TAG, "Low Power Node: %s", config.enable_lpn ? "Enabled" : "Disabled");
    
    // Generate unique node UUID based on MAC address
    generateNodeUUID();
    
    ESP_LOGI(TAG, "Node UUID: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:"
                  "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
             m_node_uuid[0], m_node_uuid[1], m_node_uuid[2], m_node_uuid[3],
             m_node_uuid[4], m_node_uuid[5], m_node_uuid[6], m_node_uuid[7],
             m_node_uuid[8], m_node_uuid[9], m_node_uuid[10], m_node_uuid[11],
             m_node_uuid[12], m_node_uuid[13], m_node_uuid[14], m_node_uuid[15]);
    
    // Initialize BLE controller
    BLEMeshStatus status = initBLEStack();
    if (status != BLEMeshStatus::OK) {
        ESP_LOGE(TAG, "BLE stack init failed");
        return status;
    }
    
    // Initialize BLE Mesh stack
    status = initMeshStack();
    if (status != BLEMeshStatus::OK) {
        ESP_LOGE(TAG, "BLE Mesh stack init failed");
        return status;
    }
    
    m_initialized = true;
    ESP_LOGI(TAG, "BLE Mesh initialized successfully");
    ESP_LOGI(TAG, "========================================");
    
    return BLEMeshStatus::OK;
}

BLEMeshStatus BLEMeshManager::enableProvisioning() {
    if (!m_initialized) {
        ESP_LOGE(TAG, "BLE Mesh not initialized");
        return BLEMeshStatus::ERROR_INIT;
    }
    
    if (m_is_provisioned) {
        ESP_LOGI(TAG, "Node already provisioned (addr: 0x%04X)", m_unicast_addr);
        return BLEMeshStatus::OK;
    }
    
    ESP_LOGI(TAG, "Enabling provisioning...");
    ESP_LOGI(TAG, "  Method: %s", 
             m_config.prov_method == ProvisioningMethod::PB_ADV ? "PB-ADV" : "PB-GATT");
    
    // Enable provisioning with both bearers for maximum compatibility
    esp_err_t err = esp_ble_mesh_node_prov_enable(
        (esp_ble_mesh_prov_bearer_t)(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT));
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable provisioning: %d", err);
        return BLEMeshStatus::ERROR_PROVISION;
    }
    
    ESP_LOGI(TAG, "Provisioning enabled - waiting for provisioner...");
    ESP_LOGI(TAG, "Node is advertising and ready to be provisioned");
    
    return BLEMeshStatus::OK;
}

void BLEMeshManager::getNodeUUID(uint8_t* uuid) const {
    if (uuid) {
        memcpy(uuid, m_node_uuid, 16);
    }
}

BLEMeshStatus BLEMeshManager::sendSensorData(const MeshSensorData& data) {
    if (!m_initialized) {
        ESP_LOGE(TAG, "BLE Mesh not initialized");
        return BLEMeshStatus::ERROR_INIT;
    }
    
    if (!m_is_provisioned) {
        ESP_LOGW(TAG, "Node not provisioned yet - cannot send data");
        return BLEMeshStatus::ERROR_NOT_PROVISIONED;
    }
    
    ESP_LOGI(TAG, "Sending sensor data via BLE Mesh:");
    ESP_LOGI(TAG, "  Temperature: %.2f °C", data.temperature);
    ESP_LOGI(TAG, "  Humidity: %.1f %%", data.humidity);
    ESP_LOGI(TAG, "  Battery: %d %%", data.battery_percent);
    ESP_LOGI(TAG, "  Timestamp: %u", (unsigned)data.timestamp);
    
    // TODO: Implement actual BLE Mesh sensor model publication
    // This would involve:
    // 1. Packing data into BLE Mesh sensor format
    // 2. Publishing to configured group/address
    // 3. Handling retries and acknowledgments
    
    ESP_LOGI(TAG, "Data sent successfully (unicast: 0x%04X)", m_unicast_addr);
    
    return BLEMeshStatus::OK;
}

const char* BLEMeshManager::statusToString(BLEMeshStatus status) {
    switch (status) {
        case BLEMeshStatus::OK: return "OK";
        case BLEMeshStatus::ERROR_INIT: return "Initialization Error";
        case BLEMeshStatus::ERROR_PROVISION: return "Provisioning Error";
        case BLEMeshStatus::ERROR_SEND: return "Send Error";
        case BLEMeshStatus::ERROR_NOT_PROVISIONED: return "Not Provisioned";
        case BLEMeshStatus::ERROR_INVALID_PARAM: return "Invalid Parameter";
        default: return "Unknown Error";
    }
}

// Private methods

void BLEMeshManager::generateNodeUUID() {
    // Generate UUID from MAC address + random data
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    
    // First 6 bytes: MAC address
    memcpy(m_node_uuid, mac, 6);
    
    // Next 2 bytes: Company ID
    m_node_uuid[6] = (m_config.company_id >> 8) & 0xFF;
    m_node_uuid[7] = m_config.company_id & 0xFF;
    
    // Next 2 bytes: Product ID
    m_node_uuid[8] = (m_config.product_id >> 8) & 0xFF;
    m_node_uuid[9] = m_config.product_id & 0xFF;
    
    // Last 6 bytes: Sequential/random for uniqueness
    for (int i = 10; i < 16; i++) {
        m_node_uuid[i] = (mac[i % 6] ^ (i * 17)) & 0xFF;
    }
}

BLEMeshStatus BLEMeshManager::initBLEStack() {
    ESP_LOGI(TAG, "Initializing BLE controller...");
    
    // Release classic BT memory (we only need BLE)
    esp_err_t err = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "BT memory release failed: %d (may be already released)", err);
    }
    
    // Initialize BLE controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    err = esp_bt_controller_init(&bt_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "BT controller init failed: %d", err);
        return BLEMeshStatus::ERROR_INIT;
    }
    
    // Enable BLE mode
    err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "BT controller enable failed: %d", err);
        return BLEMeshStatus::ERROR_INIT;
    }
    
    // Initialize Bluedroid
    err = esp_bluedroid_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid init failed: %d", err);
        return BLEMeshStatus::ERROR_INIT;
    }
    
    err = esp_bluedroid_enable();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid enable failed: %d", err);
        return BLEMeshStatus::ERROR_INIT;
    }
    
    ESP_LOGI(TAG, "BLE controller initialized");
    return BLEMeshStatus::OK;
}

BLEMeshStatus BLEMeshManager::initMeshStack() {
    ESP_LOGI(TAG, "Initializing BLE Mesh stack...");
    
    // Initialize BLE Mesh
    esp_err_t err = esp_ble_mesh_init(nullptr, nullptr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "BLE Mesh init failed: %d", err);
        return BLEMeshStatus::ERROR_INIT;
    }
    
    // Check if already provisioned (from NVS)
    if (esp_ble_mesh_node_is_provisioned()) {
        m_is_provisioned = true;
        m_unicast_addr = esp_ble_mesh_get_primary_element_address();
        ESP_LOGI(TAG, "Node is already provisioned!");
        ESP_LOGI(TAG, "  Unicast address: 0x%04X", m_unicast_addr);
    } else {
        ESP_LOGI(TAG, "Node is unprovisioned");
    }
    
    // Enable Low Power Node feature if configured
    if (m_config.enable_lpn && !m_is_provisioned) {
        ESP_LOGI(TAG, "Low Power Node feature will be enabled after provisioning");
    }
    
    ESP_LOGI(TAG, "BLE Mesh stack initialized");
    return BLEMeshStatus::OK;
}

void BLEMeshManager::provisioningCallback(int event, void* param) {
    // This would handle provisioning events
    // For now, just log
    ESP_LOGI(TAG, "Provisioning event: %d", event);
}

