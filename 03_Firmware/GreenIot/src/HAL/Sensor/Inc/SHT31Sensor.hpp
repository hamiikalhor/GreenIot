/**
 * @file SHT31Sensor.hpp
 * @brief SHT31 sensor driver (Sensirion) - C++ implementation
 * 
 * Implements ISensor interface for SHT31 temperature & humidity sensor
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#ifndef SHT31_SENSOR_HPP
#define SHT31_SENSOR_HPP

#include "ISensor.hpp"
#include <cstdint>

/**
 * @brief SHT31 sensor driver class
 */
class SHT31Sensor : public ISensor {
public:
    SHT31Sensor();
    ~SHT31Sensor() override;
    
    // ISensor interface implementation
    SensorStatus init() override;
    SensorStatus deinit() override;
    SensorStatus triggerMeasurement() override;
    SensorStatus read(SensorData& data) override;
    SensorStatus sleep() override;
    SensorStatus wakeup() override;
    SensorStatus selfTest() override;
    SensorStatus reset() override;
    SensorStatus configure(const SensorConfig& config) override;
    const SensorInfo& getInfo() const override;
    
private:
    // SHT31 Hardware Constants
    static constexpr uint8_t I2C_ADDR_DEFAULT = 0x44;
    static constexpr uint8_t I2C_ADDR_ALT = 0x45;
    
    static constexpr uint16_t CMD_MEAS_HIGH = 0x2C06;
    static constexpr uint16_t CMD_MEAS_MED  = 0x2C0D;
    static constexpr uint16_t CMD_MEAS_LOW  = 0x2C10;
    static constexpr uint16_t CMD_SOFT_RESET = 0x30A2;
    static constexpr uint16_t CMD_HEATER_ON  = 0x306D;
    static constexpr uint16_t CMD_HEATER_OFF = 0x3066;
    static constexpr uint16_t CMD_READ_STATUS = 0xF32D;
    
    static constexpr uint16_t MEAS_TIME_HIGH_MS = 15;
    static constexpr uint16_t MEAS_TIME_MED_MS  = 6;
    static constexpr uint16_t MEAS_TIME_LOW_MS  = 4;
    static constexpr uint16_t RESET_TIME_MS = 2;
    
    // Private state
    bool m_initialized;
    uint8_t m_i2c_address;
    SensorConfig m_config;
    uint32_t m_last_meas_time;
    
    // Private helper methods
    uint8_t calculateCRC8(const uint8_t* data, uint8_t len);
    SensorStatus sendCommand(uint16_t command);
    void convertRawData(uint16_t temp_raw, uint16_t hum_raw, SensorData& data);
};

#endif // SHT31_SENSOR_HPP

