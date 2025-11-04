/**
 * @file ISensor.hpp
 * @brief Abstract sensor interface (C++ version)
 * 
 * This interface allows swapping sensor hardware (SHT31, AHT20, HDC2080, etc.)
 * without modifying application code. All sensors must implement this interface.
 * 
 * Architecture Layer: HAL (Hardware Abstraction Layer)
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#ifndef ISENSOR_HPP
#define ISENSOR_HPP

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

/**
 * @brief Sensor status codes
 */
enum class SensorStatus {
    OK = 0,
    ERROR_INIT,
    ERROR_COMM,
    ERROR_TIMEOUT,
    ERROR_CRC,
    ERROR_OUT_OF_RANGE,
    ERROR_NOT_READY,
    ERROR_INVALID_PARAM
};

/**
 * @brief Sensor data structure (standardized output)
 */
struct SensorData {
    float temperature_celsius;
    float humidity_percent;
    uint32_t timestamp;
    uint8_t quality_flags;  // [7]=temp_valid, [6]=hum_valid
    
    bool isValid() const {
        return (quality_flags & 0xC0) == 0xC0;
    }
};

/**
 * @brief Sensor configuration parameters
 */
struct SensorConfig {
    uint8_t precision;         // 0=low, 1=medium, 2=high
    float temp_offset_celsius;
    float hum_offset_percent;
    bool enable_heater;
    
    // Default constructor
    SensorConfig() 
        : precision(2)
        , temp_offset_celsius(0.0f)
        , hum_offset_percent(0.0f)
        , enable_heater(false) {}
};

/**
 * @brief Sensor information (metadata)
 */
struct SensorInfo {
    std::string name;
    std::string manufacturer;
    float temp_min;
    float temp_max;
    float hum_min;
    float hum_max;
    float temp_accuracy;
    float hum_accuracy;
    uint16_t measurement_time_ms;
    uint16_t power_active_ua;
    uint16_t power_sleep_ua;
};

/**
 * @brief Abstract sensor interface (pure virtual base class)
 * 
 * All sensor drivers must inherit from this interface and implement all methods.
 */
class ISensor {
public:
    virtual ~ISensor() = default;
    
    /**
     * @brief Initialize sensor hardware
     * @return SensorStatus::OK on success
     */
    virtual SensorStatus init() = 0;
    
    /**
     * @brief Deinitialize sensor (release resources)
     * @return SensorStatus::OK on success
     */
    virtual SensorStatus deinit() = 0;
    
    /**
     * @brief Trigger a measurement (may be asynchronous)
     * @return SensorStatus::OK on success
     */
    virtual SensorStatus triggerMeasurement() = 0;
    
    /**
     * @brief Read sensor data (blocking or after trigger)
     * @param data Reference to store results
     * @return SensorStatus::OK on success
     */
    virtual SensorStatus read(SensorData& data) = 0;
    
    /**
     * @brief Enter low-power sleep mode
     * @return SensorStatus::OK on success
     */
    virtual SensorStatus sleep() = 0;
    
    /**
     * @brief Wake from sleep mode
     * @return SensorStatus::OK on success
     */
    virtual SensorStatus wakeup() = 0;
    
    /**
     * @brief Perform sensor self-test (if supported)
     * @return SensorStatus::OK if test passes
     */
    virtual SensorStatus selfTest() = 0;
    
    /**
     * @brief Reset sensor to default state
     * @return SensorStatus::OK on success
     */
    virtual SensorStatus reset() = 0;
    
    /**
     * @brief Apply configuration parameters
     * @param config Configuration to apply
     * @return SensorStatus::OK on success
     */
    virtual SensorStatus configure(const SensorConfig& config) = 0;
    
    /**
     * @brief Get sensor information (metadata)
     * @return Sensor info structure
     */
    virtual const SensorInfo& getInfo() const = 0;
    
    /**
     * @brief Get sensor status as string (for debugging)
     * @param status Status code
     * @return String representation
     */
    static const char* statusToString(SensorStatus status) {
        switch (status) {
            case SensorStatus::OK:                 return "OK";
            case SensorStatus::ERROR_INIT:         return "Initialization Failed";
            case SensorStatus::ERROR_COMM:         return "Communication Error";
            case SensorStatus::ERROR_TIMEOUT:      return "Timeout";
            case SensorStatus::ERROR_CRC:          return "CRC Error";
            case SensorStatus::ERROR_OUT_OF_RANGE: return "Out of Range";
            case SensorStatus::ERROR_NOT_READY:    return "Not Ready";
            case SensorStatus::ERROR_INVALID_PARAM: return "Invalid Parameter";
            default:                               return "Unknown Error";
        }
    }
};

/**
 * @brief Sensor factory class
 */
class SensorFactory {
public:
    /**
     * @brief Create sensor instance by name
     * @param name Sensor name (e.g., "SHT31", "AHT20")
     * @return Unique pointer to sensor, or nullptr if not found
     */
    static std::unique_ptr<ISensor> create(const std::string& name);
    
    /**
     * @brief Get list of available sensors
     * @return Vector of sensor names
     */
    static std::vector<std::string> getAvailableSensors();
};

#endif // ISENSOR_HPP

