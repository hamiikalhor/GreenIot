/**
 * @file sensor_interface.c
 * @brief Sensor registry and interface selection
 */

#include "HAL/Sensor/sensor_interface.h"
#include <string.h>

extern const sensor_interface_t sht31_driver;
// extern const sensor_interface_t aht20_driver;

typedef struct {
    const char *name;
    const sensor_interface_t *interface;
} sensor_registry_entry_t;

static const sensor_registry_entry_t sensor_registry[] = {
    { "SHT31", &sht31_driver },
};

#define SENSOR_COUNT (sizeof(sensor_registry) / sizeof(sensor_registry[0]))

const sensor_interface_t* sensor_get_interface(const char *sensor_name) {
    if (sensor_name == NULL) return NULL;
    
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        if (strcmp(sensor_name, sensor_registry[i].name) == 0) {
            return sensor_registry[i].interface;
        }
    }
    return NULL;
}

const char** sensor_get_available_sensors(uint8_t *count) {
    static const char *sensor_names[SENSOR_COUNT];
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        sensor_names[i] = sensor_registry[i].name;
    }
    if (count) *count = SENSOR_COUNT;
    return sensor_names;
}

const char* sensor_status_to_string(sensor_status_t status) {
    switch (status) {
        case SENSOR_OK: return "OK";
        case SENSOR_ERROR_INIT: return "Initialization Failed";
        case SENSOR_ERROR_COMM: return "Communication Error";
        case SENSOR_ERROR_TIMEOUT: return "Timeout";
        case SENSOR_ERROR_CRC: return "CRC Mismatch";
        case SENSOR_ERROR_OUT_OF_RANGE: return "Out of Range";
        case SENSOR_ERROR_NOT_READY: return "Not Ready";
        case SENSOR_ERROR_INVALID_PARAM: return "Invalid Parameter";
        default: return "Unknown Error";
    }
}

