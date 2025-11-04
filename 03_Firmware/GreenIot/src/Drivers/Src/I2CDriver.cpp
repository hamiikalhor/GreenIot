/**
 * @file I2CDriver.cpp
 * @brief I2C peripheral driver implementation for ESP-IDF
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#include "I2CDriver.hpp"
#include "driver/i2c.h"
#include "esp_log.h"

static const char* TAG = "I2C";

#define I2C_MASTER_NUM I2C_NUM_0
#define ACK_CHECK_EN 1
#define ACK_CHECK_DIS 0

I2CDriver& I2CDriver::getInstance() {
    static I2CDriver instance;
    return instance;
}

I2CStatus I2CDriver::init(const I2CConfig& config) {
    m_config = config;
    
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = static_cast<gpio_num_t>(config.sda_pin);
    conf.scl_io_num = static_cast<gpio_num_t>(config.scl_pin);
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = config.frequency_hz;
    conf.clk_flags = 0;
    
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed");
        return I2CStatus::ERROR_INIT;
    }
    
    err = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed");
        return I2CStatus::ERROR_INIT;
    }
    
    m_initialized = true;
    ESP_LOGI(TAG, "I2C initialized (SDA=%d, SCL=%d, %d Hz)", 
             config.sda_pin, config.scl_pin, (int)config.frequency_hz);
    return I2CStatus::OK;
}

I2CStatus I2CDriver::deinit() {
    if (!m_initialized) return I2CStatus::OK;
    
    i2c_driver_delete(I2C_MASTER_NUM);
    m_initialized = false;
    return I2CStatus::OK;
}

I2CStatus I2CDriver::write(uint8_t device_addr, const uint8_t* data, uint16_t len) {
    if (!m_initialized) return I2CStatus::ERROR_INIT;
    if (data == nullptr || len == 0) return I2CStatus::ERROR_INVALID_PARAM;
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write(cmd, const_cast<uint8_t*>(data), len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 
                                         pdMS_TO_TICKS(m_config.timeout_ms));
    i2c_cmd_link_delete(cmd);
    
    if (ret == ESP_OK) return I2CStatus::OK;
    if (ret == ESP_ERR_TIMEOUT) return I2CStatus::ERROR_TIMEOUT;
    return I2CStatus::ERROR_NACK_ADDR;
}

I2CStatus I2CDriver::read(uint8_t device_addr, uint8_t* data, uint16_t len) {
    if (!m_initialized) return I2CStatus::ERROR_INIT;
    if (data == nullptr || len == 0) return I2CStatus::ERROR_INVALID_PARAM;
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 
                                         pdMS_TO_TICKS(m_config.timeout_ms));
    i2c_cmd_link_delete(cmd);
    
    if (ret == ESP_OK) return I2CStatus::OK;
    if (ret == ESP_ERR_TIMEOUT) return I2CStatus::ERROR_TIMEOUT;
    return I2CStatus::ERROR_NACK_ADDR;
}

I2CStatus I2CDriver::writeRead(uint8_t device_addr, const uint8_t* write_data, uint16_t write_len, 
                                uint8_t* read_data, uint16_t read_len) {
    if (!m_initialized) return I2CStatus::ERROR_INIT;
    if (write_data == nullptr || write_len == 0 || read_data == nullptr || read_len == 0) {
        return I2CStatus::ERROR_INVALID_PARAM;
    }
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // Write phase
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write(cmd, const_cast<uint8_t*>(write_data), write_len, ACK_CHECK_EN);
    
    // Read phase (repeated start)
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    
    if (read_len > 1) {
        i2c_master_read(cmd, read_data, read_len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, read_data + read_len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 
                                         pdMS_TO_TICKS(m_config.timeout_ms));
    i2c_cmd_link_delete(cmd);
    
    if (ret == ESP_OK) return I2CStatus::OK;
    if (ret == ESP_ERR_TIMEOUT) return I2CStatus::ERROR_TIMEOUT;
    return I2CStatus::ERROR_NACK_ADDR;
}

bool I2CDriver::isDevicePresent(uint8_t device_addr) {
    if (!m_initialized) return false;
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);
    
    return (ret == ESP_OK);
}

std::vector<uint8_t> I2CDriver::scan() {
    std::vector<uint8_t> devices;
    
    if (!m_initialized) return devices;
    
    for (uint8_t addr = 1; addr < 127; addr++) {
        if (isDevicePresent(addr)) {
            devices.push_back(addr);
        }
    }
    
    return devices;
}

const char* I2CDriver::statusToString(I2CStatus status) {
    switch (status) {
        case I2CStatus::OK: return "OK";
        case I2CStatus::ERROR_INIT: return "Initialization Error";
        case I2CStatus::ERROR_NACK_ADDR: return "NACK on Address";
        case I2CStatus::ERROR_NACK_DATA: return "NACK on Data";
        case I2CStatus::ERROR_TIMEOUT: return "Timeout";
        case I2CStatus::ERROR_BUS_BUSY: return "Bus Busy";
        case I2CStatus::ERROR_INVALID_PARAM: return "Invalid Parameter";
        default: return "Unknown Error";
    }
}

