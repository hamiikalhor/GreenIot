/**
 * @file mock_i2c.cpp
 * @brief Mock I2C Driver for Native Testing
 * 
 * Provides mock implementations of I2C functions for native (PC) testing.
 */

#include "Drivers/i2c_driver.h"
#include <stddef.h>
#include <string.h>

// Mock I2C state
static bool mock_i2c_initialized = false;
static uint8_t mock_device_addresses[10] = {0x44, 0x45};  // Mock SHT31 addresses
static uint8_t mock_device_count = 2;

extern "C" {

i2c_status_t i2c_init(const i2c_config_t *config) {
    if (config == NULL) {
        return I2C_ERROR_INVALID_PARAM;
    }
    
    mock_i2c_initialized = true;
    return I2C_OK;
}

i2c_status_t i2c_deinit(void) {
    mock_i2c_initialized = false;
    return I2C_OK;
}

i2c_status_t i2c_write(uint8_t device_addr, const uint8_t *data, uint16_t len) {
    if (data == NULL) {
        return I2C_ERROR_INVALID_PARAM;
    }
    
    if (!mock_i2c_initialized) {
        return I2C_ERROR_INIT;
    }
    
    // Mock successful write
    return I2C_OK;
}

i2c_status_t i2c_read(uint8_t device_addr, uint8_t *data, uint16_t len) {
    if (data == NULL) {
        return I2C_ERROR_INVALID_PARAM;
    }
    
    if (!mock_i2c_initialized) {
        return I2C_ERROR_INIT;
    }
    
    // Mock sensor data (SHT31 format)
    // Temperature: 25°C, Humidity: 60%
    if (len >= 6) {
        data[0] = 0x63;  // Temp MSB
        data[1] = 0x00;  // Temp LSB
        data[2] = 0x00;  // Temp CRC
        data[3] = 0x5E;  // Hum MSB
        data[4] = 0x00;  // Hum LSB
        data[5] = 0x00;  // Hum CRC
    }
    
    return I2C_OK;
}

i2c_status_t i2c_write_read(uint8_t device_addr, const uint8_t *write_data, 
                            uint16_t write_len, uint8_t *read_data, uint16_t read_len) {
    if (write_data == NULL || read_data == NULL) {
        return I2C_ERROR_INVALID_PARAM;
    }
    
    // Mock write-read operation
    return I2C_OK;
}

bool i2c_device_present(uint8_t device_addr) {
    if (!mock_i2c_initialized) {
        return false;
    }
    
    // Check if address is in mock device list
    for (uint8_t i = 0; i < mock_device_count; i++) {
        if (mock_device_addresses[i] == device_addr) {
            return true;
        }
    }
    
    return false;
}

uint8_t i2c_scan(uint8_t *found_devices, uint8_t max_devices) {
    if (!mock_i2c_initialized || found_devices == NULL) {
        return 0;
    }
    
    uint8_t count = mock_device_count < max_devices ? mock_device_count : max_devices;
    
    for (uint8_t i = 0; i < count; i++) {
        found_devices[i] = mock_device_addresses[i];
    }
    
    return count;
}

const char* i2c_status_to_string(i2c_status_t status) {
    switch (status) {
        case I2C_OK:                    return "OK";
        case I2C_ERROR_INIT:            return "Initialization Error";
        case I2C_ERROR_NACK_ADDR:       return "Address Not Acknowledged";
        case I2C_ERROR_NACK_DATA:       return "Data Not Acknowledged";
        case I2C_ERROR_TIMEOUT:         return "Timeout";
        case I2C_ERROR_BUS_BUSY:        return "Bus Busy";
        case I2C_ERROR_INVALID_PARAM:   return "Invalid Parameter";
        default:                        return "Unknown Error";
    }
}

} // extern "C"

