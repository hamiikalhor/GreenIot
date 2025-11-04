/**
 * @file system.h
 * @brief System Manager for GreenIoT Vertical Farming Sensor Node
 * 
 * This class encapsulates all system initialization, configuration, and runtime
 * operations. It provides a clean interface to the main application.
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 * @version 3.0 (BLE Mesh Integration)
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include "HAL/Sensor/sensor_interface.h"
#include "HAL/Wireless/ble_mesh_interface.h"

/**
 * @brief System Manager Class
 * 
 * Manages all system resources including:
 * - Peripheral drivers (I2C)
 * - Sensor interfaces
 * - Measurement scheduling
 * - Data logging and assessment
 */
class System {
public:
    /**
     * @brief Constructor
     */
    System();
    
    /**
     * @brief Destructor
     */
    ~System();
    
    /**
     * @brief Initialize the system
     * 
     * Performs all initialization steps:
     * 1. Peripheral drivers (I2C)
     * 2. Sensor HAL layer
     * 3. Sensor configuration
     * 
     * @return true if initialization successful, false otherwise
     */
    bool init();
    
    /**
     * @brief Run one iteration of the system loop
     * 
     * Should be called periodically from main loop.
     * Handles measurement timing and execution.
     */
    void run();
    
    /**
     * @brief Stop the system and release resources
     */
    void stop();
    
    /**
     * @brief Get system uptime in milliseconds
     * @return System uptime
     */
    uint32_t getUptime() const;
    
    /**
     * @brief Get total measurement count
     * @return Number of measurements performed
     */
    uint32_t getMeasurementCount() const;
    
    /**
     * @brief Get BLE Mesh provisioning status
     * @return true if provisioned, false otherwise
     */
    bool isMeshProvisioned() const;
    
    /**
     * @brief Get BLE Mesh status
     * @return BLE Mesh status structure
     */
    ble_mesh_status_t getMeshStatus() const;

private:
    // Sensor interface
    const sensor_interface_t *sensor;
    
    // BLE Mesh enabled flag
    bool ble_mesh_enabled;
    
    // Last sensor readings (for adaptive publishing)
    sensor_data_t last_sensor_data;
    bool has_last_sensor_data;
    
    // Measurement tracking
    uint32_t measurement_count;
    uint32_t last_measurement_time;
    uint32_t last_publish_time;
    
    // System state
    bool initialized;
    
    /**
     * @brief Initialize peripheral drivers (I2C)
     * @return true if successful
     */
    bool initPeripherals();
    
    /**
     * @brief Initialize sensor HAL layer
     * @return true if successful
     */
    bool initSensor();
    
    /**
     * @brief Initialize BLE Mesh stack
     * @return true if successful
     */
    bool initBLEMesh();
    
    /**
     * @brief Print sensor information
     */
    void printSensorInfo(const sensor_info_t *info);
    
    /**
     * @brief Print sensor data with basil growth assessment
     */
    void printSensorData(const sensor_data_t *data);
    
    /**
     * @brief Perform a single measurement cycle
     */
    void performMeasurement();
    
    /**
     * @brief Publish sensor data to BLE Mesh network
     */
    void publishToMesh(const sensor_data_t *data);
    
    /**
     * @brief Check if conditions warrant immediate publishing
     * @return true if significant change detected
     */
    bool shouldPublishImmediately(const sensor_data_t *data);
};

#endif // SYSTEM_H

