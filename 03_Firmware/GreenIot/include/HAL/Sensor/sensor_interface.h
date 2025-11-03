/**
 * @file sensor_interface.h
 * @brief Abstract sensor interface (HAL Layer)
 * 
 * Architecture Layer: HARDWARE ABSTRACTION LAYER (HAL)
 * Depends on: Peripheral Driver Layer (i2c_driver, spi_driver, uart_driver)
 * Used by: Application Layer and Service Layer
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SENSOR_OK = 0,
    SENSOR_ERROR_INIT,
    SENSOR_ERROR_COMM,
    SENSOR_ERROR_TIMEOUT,
    SENSOR_ERROR_CRC,
    SENSOR_ERROR_OUT_OF_RANGE,
    SENSOR_ERROR_NOT_READY,
    SENSOR_ERROR_INVALID_PARAM
} sensor_status_t;

typedef struct {
    float temperature_celsius;
    float humidity_percent;
    uint32_t timestamp;
    uint8_t quality_flags;  // [7]=temp_valid, [6]=hum_valid
} sensor_data_t;

typedef struct {
    uint8_t precision;  // 0=low, 1=medium, 2=high
    float temp_offset_celsius;
    float hum_offset_percent;
    bool enable_heater;
} sensor_config_t;

typedef struct {
    const char *name;
    const char *manufacturer;
    const char *interface_type;
    float temp_min;
    float temp_max;
    float hum_min;
    float hum_max;
    float temp_accuracy;
    float hum_accuracy;
    uint16_t measurement_time_ms;
    uint16_t power_active_ua;
    uint16_t power_sleep_ua;
} sensor_info_t;

typedef struct {
    sensor_status_t (*init)(void);
    sensor_status_t (*deinit)(void);
    sensor_status_t (*trigger_measurement)(void);
    sensor_status_t (*read)(sensor_data_t *data);
    sensor_status_t (*sleep)(void);
    sensor_status_t (*wakeup)(void);
    sensor_status_t (*self_test)(void);
    sensor_status_t (*reset)(void);
    sensor_status_t (*configure)(const sensor_config_t *config);
    const sensor_info_t* (*get_info)(void);
} sensor_interface_t;

const sensor_interface_t* sensor_get_interface(const char *sensor_name);
const char** sensor_get_available_sensors(uint8_t *count);
const char* sensor_status_to_string(sensor_status_t status);

static inline bool sensor_data_is_valid(const sensor_data_t *data) {
    if (data == ((void*)0)) return false;  // NULL check without NULL macro
    return (data->quality_flags & 0xC0) == 0xC0;
}

#ifdef __cplusplus
}
#endif

#endif // SENSOR_INTERFACE_H

