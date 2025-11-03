/**
 * @file i2c_driver.c
 * @brief I2C peripheral driver implementation for ESP32
 */

#include "Drivers/i2c_driver.h"
#include <Wire.h>
#include <Arduino.h>

static struct {
    bool initialized;
    i2c_config_t config;
} i2c_state = {
    .initialized = false
};

i2c_status_t i2c_init(const i2c_config_t *config) {
    if (config == NULL) return I2C_ERROR_INVALID_PARAM;
    
    i2c_state.config = *config;
    Wire.begin(config->sda_pin, config->scl_pin);
    Wire.setClock(config->frequency_hz);
    Wire.setTimeOut(config->timeout_ms);
    i2c_state.initialized = true;
    
    return I2C_OK;
}

i2c_status_t i2c_deinit(void) {
    if (!i2c_state.initialized) return I2C_OK;
    Wire.end();
    i2c_state.initialized = false;
    return I2C_OK;
}

i2c_status_t i2c_write(uint8_t device_addr, const uint8_t *data, uint16_t len) {
    if (!i2c_state.initialized) return I2C_ERROR_INIT;
    if (data == NULL || len == 0) return I2C_ERROR_INVALID_PARAM;
    
    Wire.beginTransmission(device_addr);
    for (uint16_t i = 0; i < len; i++) Wire.write(data[i]);
    
    uint8_t error = Wire.endTransmission();
    switch (error) {
        case 0: return I2C_OK;
        case 2: return I2C_ERROR_NACK_ADDR;
        case 3: return I2C_ERROR_NACK_DATA;
        case 5: return I2C_ERROR_TIMEOUT;
        default: return I2C_ERROR_BUS_BUSY;
    }
}

i2c_status_t i2c_read(uint8_t device_addr, uint8_t *data, uint16_t len) {
    if (!i2c_state.initialized) return I2C_ERROR_INIT;
    if (data == NULL || len == 0) return I2C_ERROR_INVALID_PARAM;
    
    uint16_t received = Wire.requestFrom(device_addr, len);
    if (received != len) return I2C_ERROR_TIMEOUT;
    
    uint32_t start = millis();
    uint16_t idx = 0;
    while (idx < len && (millis() - start) < i2c_state.config.timeout_ms) {
        if (Wire.available()) data[idx++] = Wire.read();
    }
    
    return (idx == len) ? I2C_OK : I2C_ERROR_TIMEOUT;
}

i2c_status_t i2c_write_read(uint8_t device_addr, const uint8_t *write_data, uint16_t write_len, uint8_t *read_data, uint16_t read_len) {
    i2c_status_t status = i2c_write(device_addr, write_data, write_len);
    if (status != I2C_OK) return status;
    delayMicroseconds(100);
    return i2c_read(device_addr, read_data, read_len);
}

bool i2c_device_present(uint8_t device_addr) {
    if (!i2c_state.initialized) return false;
    Wire.beginTransmission(device_addr);
    return (Wire.endTransmission() == 0);
}

uint8_t i2c_scan(uint8_t *found_devices, uint8_t max_devices) {
    if (!i2c_state.initialized || found_devices == NULL) return 0;
    
    uint8_t count = 0;
    for (uint8_t addr = 1; addr < 127 && count < max_devices; addr++) {
        if (i2c_device_present(addr)) found_devices[count++] = addr;
        delay(1);
    }
    return count;
}

const char* i2c_status_to_string(i2c_status_t status) {
    switch (status) {
        case I2C_OK: return "OK";
        case I2C_ERROR_INIT: return "Not Initialized";
        case I2C_ERROR_NACK_ADDR: return "NACK on Address";
        case I2C_ERROR_NACK_DATA: return "NACK on Data";
        case I2C_ERROR_TIMEOUT: return "Timeout";
        case I2C_ERROR_BUS_BUSY: return "Bus Busy";
        case I2C_ERROR_INVALID_PARAM: return "Invalid Parameter";
        default: return "Unknown Error";
    }
}

