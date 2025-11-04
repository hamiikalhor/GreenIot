/**
 * @file BLEMeshManager.hpp
 * @brief BLE Mesh network manager for ESP32-C3
 * 
 * Architecture Layer: HAL (Wireless)
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 */

#ifndef BLE_MESH_MANAGER_HPP
#define BLE_MESH_MANAGER_HPP

#include <cstdint>
#include <string>

/**
 * @brief BLE Mesh status codes
 */
enum class BLEMeshStatus {
    OK = 0,
    ERROR_INIT,
    ERROR_PROVISION,
    ERROR_SEND,
    ERROR_NOT_PROVISIONED,
    ERROR_INVALID_PARAM
};

/**
 * @brief Provisioning methods
 */
enum class ProvisioningMethod {
    PB_ADV,     // Provisioning over advertising bearer
    PB_GATT     // Provisioning over GATT bearer
};

/**
 * @brief BLE Mesh configuration
 */
struct BLEMeshConfig {
    uint16_t company_id;
    uint16_t product_id;
    ProvisioningMethod prov_method;
    bool enable_lpn;  // Low Power Node feature
    
    BLEMeshConfig()
        : company_id(0x02E5)  // Espressif company ID
        , product_id(0x0001)
        , prov_method(ProvisioningMethod::PB_ADV)
        , enable_lpn(true) {}
};

/**
 * @brief Sensor data packet for BLE Mesh
 */
struct MeshSensorData {
    float temperature;
    float humidity;
    uint32_t timestamp;
    uint8_t battery_percent;
};

/**
 * @brief BLE Mesh Manager (Singleton)
 */
class BLEMeshManager {
public:
    static BLEMeshManager& getInstance();
    
    // Delete copy
    BLEMeshManager(const BLEMeshManager&) = delete;
    BLEMeshManager& operator=(const BLEMeshManager&) = delete;
    
    /**
     * @brief Initialize BLE Mesh stack
     * @param config Configuration parameters
     * @return Status code
     */
    BLEMeshStatus init(const BLEMeshConfig& config);
    
    /**
     * @brief Enable provisioning
     * @return Status code
     */
    BLEMeshStatus enableProvisioning();
    
    /**
     * @brief Check if node is provisioned
     * @return true if provisioned
     */
    bool isProvisioned() const { return m_is_provisioned; }
    
    /**
     * @brief Get node UUID
     * @param uuid Buffer to store UUID (16 bytes)
     */
    void getNodeUUID(uint8_t* uuid) const;
    
    /**
     * @brief Get unicast address (if provisioned)
     * @return Unicast address, 0 if not provisioned
     */
    uint16_t getUnicastAddress() const { return m_unicast_addr; }
    
    /**
     * @brief Send sensor data via BLE Mesh
     * @param data Sensor data to send
     * @return Status code
     */
    BLEMeshStatus sendSensorData(const MeshSensorData& data);
    
    /**
     * @brief Get mesh status as string
     */
    static const char* statusToString(BLEMeshStatus status);
    
private:
    BLEMeshManager() 
        : m_initialized(false)
        , m_is_provisioned(false)
        , m_unicast_addr(0) {}
    ~BLEMeshManager() = default;
    
    bool m_initialized;
    bool m_is_provisioned;
    uint16_t m_unicast_addr;
    BLEMeshConfig m_config;
    uint8_t m_node_uuid[16];
    
    // Private helper methods
    void generateNodeUUID();
    BLEMeshStatus initBLEStack();
    BLEMeshStatus initMeshStack();
    static void provisioningCallback(int event, void* param);
};

#endif // BLE_MESH_MANAGER_HPP

