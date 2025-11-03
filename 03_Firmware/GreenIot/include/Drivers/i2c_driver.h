/**
 * @file i2c_driver.h
 * @brief I2C peripheral driver abstraction (Peripheral Driver Layer)
 * 
 * Architecture Layer: PERIPHERAL DRIVER LAYER
 * Used by: HAL sensor drivers (sht31_driver, aht20_driver)
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    I2C_OK = 0,
    I2C_ERROR_INIT,
    I2C_ERROR_NACK_ADDR,
    I2C_ERROR_NACK_DATA,
    I2C_ERROR_TIMEOUT,
    I2C_ERROR_BUS_BUSY,
    I2C_ERROR_INVALID_PARAM
} i2c_status_t;

typedef struct {
    uint8_t sda_pin;
    uint8_t scl_pin;
    uint32_t frequency_hz;
    uint32_t timeout_ms;
} i2c_config_t;

i2c_status_t i2c_init(const i2c_config_t *config);
i2c_status_t i2c_deinit(void);
i2c_status_t i2c_write(uint8_t device_addr, const uint8_t *data, uint16_t len);
i2c_status_t i2c_read(uint8_t device_addr, uint8_t *data, uint16_t len);
i2c_status_t i2c_write_read(uint8_t device_addr, const uint8_t *write_data, uint16_t write_len, uint8_t *read_data, uint16_t read_len);
bool i2c_device_present(uint8_t device_addr);
uint8_t i2c_scan(uint8_t *found_devices, uint8_t max_devices);
const char* i2c_status_to_string(i2c_status_t status);

#ifdef __cplusplus
}
#endif

#endif // I2C_DRIVER_H

