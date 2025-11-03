/**
 * @file system.h
 * @brief System Manager for GreenIoT Vertical Farming Sensor Node
 * 
 * This class encapsulates all system initialization, configuration, and runtime
 * operations. It provides a clean interface to the main application.
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 * @version 2.1 (System Architecture)
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include "HAL/Sensor/sensor_interface.h"

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

private:
    // Sensor interface
    const sensor_interface_t *sensor;
    
    // Measurement tracking
    uint32_t measurement_count;
    uint32_t last_measurement_time;
    
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
};

#endif // SYSTEM_H

