/**
 * @file mock_ble_mesh.h
 * @brief Mock BLE Mesh Helper Functions for Testing
 * 
 * Additional functions for test verification and mock state manipulation.
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 */

#ifndef MOCK_BLE_MESH_H
#define MOCK_BLE_MESH_H

#include "HAL/Wireless/ble_mesh_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the number of times sensor data was published (for test verification)
 * @return Number of publish calls
 */
uint32_t mock_ble_mesh_get_publish_count(void);

/**
 * @brief Get the last published sensor data (for test verification)
 * @return Pointer to last published data
 */
const mesh_sensor_data_t* mock_ble_mesh_get_last_published_data(void);

/**
 * @brief Reset mock state (for test cleanup)
 */
void mock_ble_mesh_reset(void);

/**
 * @brief Simulate successful provisioning (for test scenarios)
 * @param unicast_addr Node unicast address
 * @param net_idx Network key index
 * @param app_idx Application key index
 */
void mock_ble_mesh_simulate_provisioning(uint16_t unicast_addr, uint16_t net_idx, uint16_t app_idx);

/**
 * @brief Check if provisioning is enabled (for test verification)
 * @return true if provisioning enabled, false otherwise
 */
bool mock_ble_mesh_is_provisioning_enabled(void);

#ifdef __cplusplus
}
#endif

#endif // MOCK_BLE_MESH_H

