/**
 * @file ble_mesh_interface.h
 * @brief BLE Mesh Hardware Abstraction Layer
 * 
 * This file defines the abstract interface for BLE Mesh operations, providing
 * a clean abstraction layer that allows for different BLE Mesh stack implementations
 * (ESP-BLE-MESH, Zephyr, Nordic, etc.) without changing application code.
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 * @version 1.0
 */

#ifndef BLE_MESH_INTERFACE_H
#define BLE_MESH_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Constants & Configuration
// ============================================================================

#define BLE_MESH_UUID_SIZE 16
#define BLE_MESH_DEV_NAME_MAX_LEN 32
#define BLE_MESH_NET_KEY_SIZE 16
#define BLE_MESH_APP_KEY_SIZE 16

// BLE Mesh Property IDs (from Mesh Model Specification)
#define BLE_MESH_PROP_ID_TEMPERATURE    0x004F  // Temperature 8
#define BLE_MESH_PROP_ID_HUMIDITY       0x0076  // Humidity
#define BLE_MESH_PROP_ID_BATTERY_LEVEL  0x006E  // Percentage 8

// Company ID
#define BLE_MESH_COMPANY_ID_ESPRESSIF   0x02E5

// ============================================================================
// Enumerations
// ============================================================================

/**
 * @brief BLE Mesh operation status codes
 */
typedef enum {
    BLE_MESH_OK = 0,
    BLE_MESH_ERROR_INVALID_PARAM,
    BLE_MESH_ERROR_NOT_INITIALIZED,
    BLE_MESH_ERROR_ALREADY_INITIALIZED,
    BLE_MESH_ERROR_NO_MEMORY,
    BLE_MESH_ERROR_TIMEOUT,
    BLE_MESH_ERROR_NOT_PROVISIONED,
    BLE_MESH_ERROR_ALREADY_PROVISIONED,
    BLE_MESH_ERROR_PUBLISH_FAILED,
    BLE_MESH_ERROR_UNKNOWN
} ble_mesh_err_t;

/**
 * @brief Provisioning state
 */
typedef enum {
    BLE_MESH_PROV_STATE_IDLE = 0,
    BLE_MESH_PROV_STATE_LINK_OPEN,
    BLE_MESH_PROV_STATE_LINK_ACTIVE,
    BLE_MESH_PROV_STATE_SUCCESS,
    BLE_MESH_PROV_STATE_FAILED
} ble_mesh_prov_state_t;

/**
 * @brief Node features
 */
typedef enum {
    BLE_MESH_FEATURE_RELAY = (1 << 0),
    BLE_MESH_FEATURE_PROXY = (1 << 1),
    BLE_MESH_FEATURE_FRIEND = (1 << 2),
    BLE_MESH_FEATURE_LOW_POWER = (1 << 3)
} ble_mesh_features_t;

// ============================================================================
// Data Structures
// ============================================================================

/**
 * @brief BLE Mesh configuration
 */
typedef struct {
    uint8_t uuid[BLE_MESH_UUID_SIZE];           ///< Device UUID
    char device_name[BLE_MESH_DEV_NAME_MAX_LEN]; ///< Device name
    uint16_t company_id;                        ///< Company identifier
    uint16_t product_id;                        ///< Product identifier
    uint16_t version_id;                        ///< Version identifier
    uint32_t features;                          ///< Enabled features (bitmask)
    bool enable_pb_adv;                         ///< Enable PB-ADV provisioning
    bool enable_pb_gatt;                        ///< Enable PB-GATT provisioning
    bool enable_lpn;                            ///< Enable Low Power Node
    uint32_t lpn_poll_interval_ms;              ///< LPN poll interval
} ble_mesh_config_t;

/**
 * @brief Sensor data for BLE Mesh publication
 */
typedef struct {
    uint16_t property_id;                       ///< Property ID (temp/humidity)
    uint8_t raw_value[4];                       ///< Raw sensor data
    uint8_t raw_value_len;                      ///< Length of raw data
    uint32_t timestamp;                         ///< Measurement timestamp
} ble_mesh_sensor_data_t;

/**
 * @brief Mesh sensor data packet (multiple properties)
 */
typedef struct {
    float temperature_celsius;                  ///< Temperature in °C
    float humidity_percent;                     ///< Humidity in %
    uint8_t battery_level;                      ///< Battery level (0-100%)
    uint32_t timestamp;                         ///< Timestamp (milliseconds)
} mesh_sensor_data_t;

/**
 * @brief BLE Mesh status information
 */
typedef struct {
    bool initialized;                           ///< Initialization status
    bool provisioned;                           ///< Provisioning status
    uint16_t unicast_addr;                      ///< Node unicast address
    uint16_t net_idx;                           ///< Network key index
    uint16_t app_idx;                           ///< Application key index
    ble_mesh_prov_state_t prov_state;          ///< Provisioning state
    uint32_t features;                          ///< Enabled features
    uint32_t uptime_ms;                         ///< Uptime in milliseconds
} ble_mesh_status_t;

/**
 * @brief Provisioning event callback
 */
typedef void (*ble_mesh_prov_callback_t)(ble_mesh_prov_state_t state, uint16_t addr);

/**
 * @brief Sensor data received callback
 */
typedef void (*ble_mesh_sensor_callback_t)(uint16_t src_addr, mesh_sensor_data_t *data);

// ============================================================================
// Function Prototypes
// ============================================================================

/**
 * @brief Initialize BLE Mesh stack
 * 
 * @param config Configuration structure
 * @return ble_mesh_err_t Status code
 */
ble_mesh_err_t ble_mesh_init(const ble_mesh_config_t *config);

/**
 * @brief Deinitialize BLE Mesh stack
 * 
 * @return ble_mesh_err_t Status code
 */
ble_mesh_err_t ble_mesh_deinit(void);

/**
 * @brief Enable unprovisioned device beacon (for provisioning)
 * 
 * @return ble_mesh_err_t Status code
 */
ble_mesh_err_t ble_mesh_enable_provisioning(void);

/**
 * @brief Disable provisioning beacon
 * 
 * @return ble_mesh_err_t Status code
 */
ble_mesh_err_t ble_mesh_disable_provisioning(void);

/**
 * @brief Check if node is provisioned
 * 
 * @return true if provisioned, false otherwise
 */
bool ble_mesh_is_provisioned(void);

/**
 * @brief Publish sensor data to mesh network
 * 
 * @param data Sensor data to publish
 * @return ble_mesh_err_t Status code
 */
ble_mesh_err_t ble_mesh_publish_sensor_data(const mesh_sensor_data_t *data);

/**
 * @brief Get current mesh status
 * 
 * @param status Pointer to status structure
 * @return ble_mesh_err_t Status code
 */
ble_mesh_err_t ble_mesh_get_status(ble_mesh_status_t *status);

/**
 * @brief Register provisioning event callback
 * 
 * @param callback Callback function
 * @return ble_mesh_err_t Status code
 */
ble_mesh_err_t ble_mesh_register_prov_callback(ble_mesh_prov_callback_t callback);

/**
 * @brief Register sensor data received callback
 * 
 * @param callback Callback function
 * @return ble_mesh_err_t Status code
 */
ble_mesh_err_t ble_mesh_register_sensor_callback(ble_mesh_sensor_callback_t callback);

/**
 * @brief Reset node to factory settings (unprovision)
 * 
 * @return ble_mesh_err_t Status code
 */
ble_mesh_err_t ble_mesh_factory_reset(void);

/**
 * @brief Convert status code to string
 * 
 * @param status Status code
 * @return const char* Status string
 */
const char* ble_mesh_status_to_string(ble_mesh_err_t status);

/**
 * @brief Convert provisioning state to string
 * 
 * @param state Provisioning state
 * @return const char* State string
 */
const char* ble_mesh_prov_state_to_string(ble_mesh_prov_state_t state);

#ifdef __cplusplus
}
#endif

#endif // BLE_MESH_INTERFACE_H

