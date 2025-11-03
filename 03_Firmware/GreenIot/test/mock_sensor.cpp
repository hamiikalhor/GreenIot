/**
 * @file mock_sensor.cpp
 * @brief Mock Sensor Driver for Native Testing
 * 
 * Provides a mock SHT31 sensor implementation for testing the sensor interface.
 */

#include "HAL/Sensor/sensor_interface.h"
#include <stddef.h>

// Mock sensor data
static sensor_data_t mock_sensor_data = {
    .temperature_celsius = 25.0f,
    .humidity_percent = 60.0f,
    .timestamp = 1000,
    .quality_flags = 0xC0  // Both valid
};

static bool mock_sensor_initialized = false;

// Mock sensor functions
static sensor_status_t mock_sht31_init(void) {
    mock_sensor_initialized = true;
    return SENSOR_OK;
}

static sensor_status_t mock_sht31_deinit(void) {
    mock_sensor_initialized = false;
    return SENSOR_OK;
}

static sensor_status_t mock_sht31_trigger_measurement(void) {
    if (!mock_sensor_initialized) {
        return SENSOR_ERROR_INIT;
    }
    return SENSOR_OK;
}

static sensor_status_t mock_sht31_read(sensor_data_t *data) {
    if (!mock_sensor_initialized) {
        return SENSOR_ERROR_INIT;
    }
    
    if (data == NULL) {
        return SENSOR_ERROR_INVALID_PARAM;
    }
    
    *data = mock_sensor_data;
    return SENSOR_OK;
}

static sensor_status_t mock_sht31_sleep(void) {
    return SENSOR_OK;
}

static sensor_status_t mock_sht31_wakeup(void) {
    return SENSOR_OK;
}

static sensor_status_t mock_sht31_self_test(void) {
    return SENSOR_OK;
}

static sensor_status_t mock_sht31_reset(void) {
    return SENSOR_OK;
}

static sensor_status_t mock_sht31_configure(const sensor_config_t *config) {
    if (config == NULL) {
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return SENSOR_OK;
}

static const sensor_info_t* mock_sht31_get_info(void) {
    static const sensor_info_t info = {
        .name = "SHT31 (Mock)",
        .manufacturer = "Sensirion",
        .interface_type = "I2C",
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
    return &info;
}

// Export mock sensor driver
extern "C" const sensor_interface_t sht31_driver = {
    .init = mock_sht31_init,
    .deinit = mock_sht31_deinit,
    .trigger_measurement = mock_sht31_trigger_measurement,
    .read = mock_sht31_read,
    .sleep = mock_sht31_sleep,
    .wakeup = mock_sht31_wakeup,
    .self_test = mock_sht31_self_test,
    .reset = mock_sht31_reset,
    .configure = mock_sht31_configure,
    .get_info = mock_sht31_get_info
};

