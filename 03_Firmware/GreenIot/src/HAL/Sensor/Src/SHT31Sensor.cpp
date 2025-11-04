/**
 * @file SHT31Sensor.cpp
 * @brief SHT31 sensor driver implementation
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#include "SHT31Sensor.hpp"
#include "I2CDriver.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstring>

static const char* TAG = "SHT31";

SHT31Sensor::SHT31Sensor()
    : m_initialized(false)
    , m_i2c_address(I2C_ADDR_DEFAULT)
    , m_last_meas_time(0)
{
    m_config.precision = 2;  // High precision
    m_config.temp_offset_celsius = 0.0f;
    m_config.hum_offset_percent = 0.0f;
    m_config.enable_heater = false;
}

SHT31Sensor::~SHT31Sensor() {
    deinit();
}

SensorStatus SHT31Sensor::init() {
    ESP_LOGI(TAG, "Initializing SHT31 sensor");
    
    // Try default address
    m_i2c_address = I2C_ADDR_DEFAULT;
    
    // Send soft reset
    SensorStatus status = sendCommand(CMD_SOFT_RESET);
    if (status != SensorStatus::OK) {
        // Try alternate address
        ESP_LOGW(TAG, "SHT31 not found at 0x44, trying 0x45");
        m_i2c_address = I2C_ADDR_ALT;
        status = sendCommand(CMD_SOFT_RESET);
        if (status != SensorStatus::OK) {
            ESP_LOGE(TAG, "SHT31 not found on I2C bus");
            return SensorStatus::ERROR_INIT;
        }
    }
    
    // Wait for reset
    vTaskDelay(pdMS_TO_TICKS(RESET_TIME_MS));
    
    m_initialized = true;
    ESP_LOGI(TAG, "SHT31 initialized at address 0x%02X", m_i2c_address);
    return SensorStatus::OK;
}

SensorStatus SHT31Sensor::deinit() {
    m_initialized = false;
    ESP_LOGI(TAG, "SHT31 deinitialized");
    return SensorStatus::OK;
}

SensorStatus SHT31Sensor::triggerMeasurement() {
    if (!m_initialized) {
        return SensorStatus::ERROR_NOT_READY;
    }
    
    // Select command based on precision
    uint16_t cmd;
    switch (m_config.precision) {
        case 0:  cmd = CMD_MEAS_LOW; break;
        case 1:  cmd = CMD_MEAS_MED; break;
        case 2:  cmd = CMD_MEAS_HIGH; break;
        default: cmd = CMD_MEAS_HIGH; break;
    }
    
    SensorStatus status = sendCommand(cmd);
    if (status == SensorStatus::OK) {
        m_last_meas_time = static_cast<uint32_t>(esp_timer_get_time() / 1000);
    }
    return status;
}

SensorStatus SHT31Sensor::read(SensorData& data) {
    if (!m_initialized) {
        return SensorStatus::ERROR_NOT_READY;
    }
    
    // Wait for measurement if needed
    uint16_t meas_time = (m_config.precision == 0) ? MEAS_TIME_LOW_MS :
                         (m_config.precision == 1) ? MEAS_TIME_MED_MS : MEAS_TIME_HIGH_MS;
    
    uint32_t elapsed = static_cast<uint32_t>(esp_timer_get_time() / 1000) - m_last_meas_time;
    if (elapsed < meas_time) {
        vTaskDelay(pdMS_TO_TICKS(meas_time - elapsed));
    }
    
    // Read 6 bytes
    uint8_t read_buf[6];
    if (I2CDriver::getInstance().read(m_i2c_address, read_buf, 6) != I2CStatus::OK) {
        ESP_LOGE(TAG, "I2C read failed");
        return SensorStatus::ERROR_COMM;
    }
    
    // Verify CRCs
    if (calculateCRC8(&read_buf[0], 2) != read_buf[2]) {
        ESP_LOGE(TAG, "Temperature CRC mismatch");
        return SensorStatus::ERROR_CRC;
    }
    if (calculateCRC8(&read_buf[3], 2) != read_buf[5]) {
        ESP_LOGE(TAG, "Humidity CRC mismatch");
        return SensorStatus::ERROR_CRC;
    }
    
    // Convert raw values
    uint16_t temp_raw = (static_cast<uint16_t>(read_buf[0]) << 8) | read_buf[1];
    uint16_t hum_raw = (static_cast<uint16_t>(read_buf[3]) << 8) | read_buf[4];
    
    convertRawData(temp_raw, hum_raw, data);
    
    // Set quality flags
    data.quality_flags = 0xC0;
    data.timestamp = static_cast<uint32_t>(esp_timer_get_time() / 1000000);
    
    ESP_LOGD(TAG, "Read: %.2f°C, %.1f%% RH", data.temperature_celsius, data.humidity_percent);
    
    return SensorStatus::OK;
}

SensorStatus SHT31Sensor::sleep() {
    // SHT31 auto-sleeps
    return SensorStatus::OK;
}

SensorStatus SHT31Sensor::wakeup() {
    // SHT31 auto-wakes
    return SensorStatus::OK;
}

SensorStatus SHT31Sensor::selfTest() {
    if (!m_initialized) {
        return SensorStatus::ERROR_NOT_READY;
    }
    
    if (sendCommand(CMD_READ_STATUS) != SensorStatus::OK) {
        return SensorStatus::ERROR_COMM;
    }
    
    uint8_t status_buf[3];
    if (I2CDriver::getInstance().read(m_i2c_address, status_buf, 3) != I2CStatus::OK) {
        return SensorStatus::ERROR_COMM;
    }
    
    uint16_t status = (static_cast<uint16_t>(status_buf[0]) << 8) | status_buf[1];
    if (status & 0x0401) {
        ESP_LOGE(TAG, "Self-test failed, status: 0x%04X", status);
        return SensorStatus::ERROR_INIT;
    }
    
    ESP_LOGI(TAG, "Self-test passed");
    return SensorStatus::OK;
}

SensorStatus SHT31Sensor::reset() {
    SensorStatus status = sendCommand(CMD_SOFT_RESET);
    if (status == SensorStatus::OK) {
        vTaskDelay(pdMS_TO_TICKS(RESET_TIME_MS));
    }
    return status;
}

SensorStatus SHT31Sensor::configure(const SensorConfig& config) {
    m_config = config;
    
    // Apply heater setting
    if (config.enable_heater) {
        return sendCommand(CMD_HEATER_ON);
    } else {
        return sendCommand(CMD_HEATER_OFF);
    }
}

const SensorInfo& SHT31Sensor::getInfo() const {
    static const SensorInfo info = {
        .name = "SHT31",
        .manufacturer = "Sensirion",
        .temp_min = -40.0f,
        .temp_max = 125.0f,
        .hum_min = 0.0f,
        .hum_max = 100.0f,
        .temp_accuracy = 0.3f,
        .hum_accuracy = 2.0f,
        .measurement_time_ms = 15,
        .power_active_ua = 800,
        .power_sleep_ua = 0
    };
    
    return info;
}

// Private helper methods

uint8_t SHT31Sensor::calculateCRC8(const uint8_t* data, uint8_t len) {
    uint8_t crc = 0xFF;
    
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = crc << 1;
            }
        }
    }
    
    return crc;
}

SensorStatus SHT31Sensor::sendCommand(uint16_t command) {
    uint8_t cmd_buf[2] = {
        static_cast<uint8_t>(command >> 8),
        static_cast<uint8_t>(command & 0xFF)
    };
    
    if (I2CDriver::getInstance().write(m_i2c_address, cmd_buf, 2) != I2CStatus::OK) {
        return SensorStatus::ERROR_COMM;
    }
    
    return SensorStatus::OK;
}

void SHT31Sensor::convertRawData(uint16_t temp_raw, uint16_t hum_raw, SensorData& data) {
    // Temperature conversion
    data.temperature_celsius = -45.0f + 175.0f * (static_cast<float>(temp_raw) / 65535.0f);
    
    // Humidity conversion
    data.humidity_percent = 100.0f * (static_cast<float>(hum_raw) / 65535.0f);
    
    // Apply calibration offsets
    data.temperature_celsius += m_config.temp_offset_celsius;
    data.humidity_percent += m_config.hum_offset_percent;
    
    // Clamp humidity
    if (data.humidity_percent < 0.0f) data.humidity_percent = 0.0f;
    if (data.humidity_percent > 100.0f) data.humidity_percent = 100.0f;
}

