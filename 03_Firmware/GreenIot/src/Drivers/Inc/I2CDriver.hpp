/**
 * @file I2CDriver.hpp
 * @brief I2C peripheral driver (Singleton pattern)
 * 
 * Architecture Layer: PERIPHERAL DRIVER LAYER
 * Used by: HAL sensor drivers
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#ifndef I2C_DRIVER_HPP
#define I2C_DRIVER_HPP

#include <cstdint>
#include <vector>

enum class I2CStatus {
    OK = 0,
    ERROR_INIT,
    ERROR_NACK_ADDR,
    ERROR_NACK_DATA,
    ERROR_TIMEOUT,
    ERROR_BUS_BUSY,
    ERROR_INVALID_PARAM
};

struct I2CConfig {
    uint8_t sda_pin;
    uint8_t scl_pin;
    uint32_t frequency_hz;
    uint32_t timeout_ms;
    
    I2CConfig()
        : sda_pin(8)
        , scl_pin(9)
        , frequency_hz(100000)
        , timeout_ms(100) {}
};

/**
 * @brief I2C Driver (Singleton pattern for single I2C bus)
 */
class I2CDriver {
public:
    // Singleton access
    static I2CDriver& getInstance();
    
    // Delete copy constructor and assignment
    I2CDriver(const I2CDriver&) = delete;
    I2CDriver& operator=(const I2CDriver&) = delete;
    
    // I2C operations
    I2CStatus init(const I2CConfig& config);
    I2CStatus deinit();
    I2CStatus write(uint8_t device_addr, const uint8_t* data, uint16_t len);
    I2CStatus read(uint8_t device_addr, uint8_t* data, uint16_t len);
    I2CStatus writeRead(uint8_t device_addr, const uint8_t* write_data, uint16_t write_len, 
                        uint8_t* read_data, uint16_t read_len);
    bool isDevicePresent(uint8_t device_addr);
    std::vector<uint8_t> scan();
    
    static const char* statusToString(I2CStatus status);
    
private:
    I2CDriver() : m_initialized(false) {}
    ~I2CDriver() = default;
    
    bool m_initialized;
    I2CConfig m_config;
};

#endif // I2C_DRIVER_HPP

