/**
 * @file sht31_driver.c
 * @brief SHT31 sensor driver implementation
 * 
 * Architecture: Uses i2c_driver (Peripheral Layer), implements sensor_interface (HAL)
 */

#include "HAL/Sensor/sensor_interface.h"
#include "Drivers/i2c_driver.h"
#include "esp_log.h"
#include <Arduino.h>
#include <string.h>

static const char *TAG_SHT31 = "SHT31";  // Log tag for SHT31 driver

#define SHT31_I2C_ADDR          0x44
#define SHT31_I2C_ADDR_ALT      0x45
#define SHT31_CMD_MEAS_HIGH     0x2C06
#define SHT31_CMD_MEAS_MED      0x2C0D
#define SHT31_CMD_MEAS_LOW      0x2C10
#define SHT31_CMD_SOFT_RESET    0x30A2
#define SHT31_CMD_HEATER_ON     0x306D
#define SHT31_CMD_HEATER_OFF    0x3066
#define SHT31_MEAS_TIME_HIGH    15
#define SHT31_MEAS_TIME_MED     6
#define SHT31_MEAS_TIME_LOW     4
#define SHT31_RESET_TIME        2

static struct {
    bool initialized;
    uint8_t i2c_address;
    sensor_config_t config;
    uint32_t last_meas_time;
} sht31_state = {
    .initialized = false,
    .i2c_address = SHT31_I2C_ADDR,
    .config = { .precision = 2, .temp_offset_celsius = 0.0f, .hum_offset_percent = 0.0f, .enable_heater = false },
    .last_meas_time = 0
};

static uint8_t sht31_crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            crc = (crc & 0x80) ? ((crc << 1) ^ 0x31) : (crc << 1);
        }
    }
    return crc;
}

static sensor_status_t sht31_send_command(uint16_t command) {
    uint8_t cmd_buf[2] = { (uint8_t)(command >> 8), (uint8_t)(command & 0xFF) };
    i2c_status_t status = i2c_write(sht31_state.i2c_address, cmd_buf, 2);
    return (status == I2C_OK) ? SENSOR_OK : ((status == I2C_ERROR_TIMEOUT) ? SENSOR_ERROR_TIMEOUT : SENSOR_ERROR_COMM);
}

static sensor_status_t sht31_init(void) {
    ESP_LOGI(TAG_SHT31, "Initializing SHT31 sensor...");
    
    // Try default address first
    if (i2c_device_present(SHT31_I2C_ADDR)) {
        sht31_state.i2c_address = SHT31_I2C_ADDR;
        ESP_LOGI(TAG_SHT31, "Found SHT31 at address 0x%02X", SHT31_I2C_ADDR);
    } else if (i2c_device_present(SHT31_I2C_ADDR_ALT)) {
        sht31_state.i2c_address = SHT31_I2C_ADDR_ALT;
        ESP_LOGI(TAG_SHT31, "Found SHT31 at address 0x%02X", SHT31_I2C_ADDR_ALT);
    } else {
        ESP_LOGE(TAG_SHT31, "SHT31 not found on I2C bus");
        return SENSOR_ERROR_INIT;
    }
    
    if (sht31_send_command(SHT31_CMD_SOFT_RESET) != SENSOR_OK) {
        ESP_LOGE(TAG_SHT31, "Failed to reset SHT31");
        return SENSOR_ERROR_INIT;
    }
    delay(SHT31_RESET_TIME);
    
    sht31_state.initialized = true;
    ESP_LOGI(TAG_SHT31, "SHT31 initialized successfully");
    return SENSOR_OK;
}

static sensor_status_t sht31_deinit(void) {
    sht31_state.initialized = false;
    return SENSOR_OK;
}

static sensor_status_t sht31_trigger_measurement(void) {
    if (!sht31_state.initialized) return SENSOR_ERROR_NOT_READY;
    
    uint16_t cmd = (sht31_state.config.precision == 0) ? SHT31_CMD_MEAS_LOW :
                   (sht31_state.config.precision == 1) ? SHT31_CMD_MEAS_MED : SHT31_CMD_MEAS_HIGH;
    
    sensor_status_t status = sht31_send_command(cmd);
    if (status == SENSOR_OK) sht31_state.last_meas_time = millis();
    return status;
}

static sensor_status_t sht31_read(sensor_data_t *data) {
    if (!sht31_state.initialized) return SENSOR_ERROR_NOT_READY;
    if (data == NULL) return SENSOR_ERROR_INVALID_PARAM;
    
    uint16_t meas_time = (sht31_state.config.precision == 0) ? SHT31_MEAS_TIME_LOW :
                         (sht31_state.config.precision == 1) ? SHT31_MEAS_TIME_MED : SHT31_MEAS_TIME_HIGH;
    
    uint32_t elapsed = millis() - sht31_state.last_meas_time;
    if (elapsed < meas_time) delay(meas_time - elapsed);
    
    uint8_t buf[6];
    if (i2c_read(sht31_state.i2c_address, buf, 6) != I2C_OK) {
        ESP_LOGE(TAG_SHT31, "I2C read failed");
        return SENSOR_ERROR_COMM;
    }
    
    // Verify CRC
    if (sht31_crc8(&buf[0], 2) != buf[2] || sht31_crc8(&buf[3], 2) != buf[5]) {
        ESP_LOGE(TAG_SHT31, "CRC mismatch");
        return SENSOR_ERROR_CRC;
    }
    
    uint16_t temp_raw = ((uint16_t)buf[0] << 8) | buf[1];
    uint16_t hum_raw = ((uint16_t)buf[3] << 8) | buf[4];
    
    data->temperature_celsius = -45.0f + 175.0f * ((float)temp_raw / 65535.0f) + sht31_state.config.temp_offset_celsius;
    data->humidity_percent = 100.0f * ((float)hum_raw / 65535.0f) + sht31_state.config.hum_offset_percent;
    
    // Clamp humidity
    if (data->humidity_percent < 0.0f) data->humidity_percent = 0.0f;
    if (data->humidity_percent > 100.0f) data->humidity_percent = 100.0f;
    
    data->quality_flags = 0xC0;
    data->timestamp = millis();
    
    ESP_LOGD(TAG_SHT31, "Read: %.2f°C, %.1f%% RH", 
             data->temperature_celsius, data->humidity_percent);
    
    return SENSOR_OK;
}

static sensor_status_t sht31_sleep(void) { return SENSOR_OK; }
static sensor_status_t sht31_wakeup(void) { return SENSOR_OK; }
static sensor_status_t sht31_self_test(void) { return SENSOR_OK; }
static sensor_status_t sht31_reset(void) {
    sensor_status_t status = sht31_send_command(SHT31_CMD_SOFT_RESET);
    if (status == SENSOR_OK) delay(2);
    return status;
}

static sensor_status_t sht31_configure(const sensor_config_t *config) {
    if (config == NULL) return SENSOR_ERROR_INVALID_PARAM;
    memcpy(&sht31_state.config, config, sizeof(sensor_config_t));
    uint16_t cmd = config->enable_heater ? SHT31_CMD_HEATER_ON : SHT31_CMD_HEATER_OFF;
    return sht31_send_command(cmd);
}

static const sensor_info_t* sht31_get_info(void) {
    static const sensor_info_t info = {
        .name = "SHT31",
        .manufacturer = "Sensirion",
        .interface_type = "I2C",
        .temp_min = -40.0f,
        .temp_max = 125.0f,
        .hum_min = 0.0f,
        .hum_max = 100.0f,
        .temp_accuracy = 0.3f,
        .hum_accuracy = 2.0f,
        .measurement_time_ms = SHT31_MEAS_TIME_HIGH,
        .power_active_ua = 800,
        .power_sleep_ua = 0
    };
    return &info;
}

extern "C" const sensor_interface_t sht31_driver = {
    .init = sht31_init,
    .deinit = sht31_deinit,
    .trigger_measurement = sht31_trigger_measurement,
    .read = sht31_read,
    .sleep = sht31_sleep,
    .wakeup = sht31_wakeup,
    .self_test = sht31_self_test,
    .reset = sht31_reset,
    .configure = sht31_configure,
    .get_info = sht31_get_info
};

