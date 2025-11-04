/**
 * @file test_sensor_cpp.cpp
 * @brief Native Unit Tests for C++ Sensor Interface (HAL Layer)
 * 
 * These tests run on PC (native) with mocked dependencies.
 */

#include <unity.h>
#include <memory>
#include <cstring>

// Mock ESP-IDF functions for native build
#ifdef NATIVE_BUILD

extern "C" {
    void esp_log_write(int level, const char* tag, const char* format, ...) {
        // No-op for tests
    }
    
    uint64_t esp_timer_get_time(void) {
        return 1000000; // 1 second in microseconds
    }
    
    void vTaskDelay(uint32_t ticks) {
        // No-op for tests
    }
}

// Mock I2C types and functions
typedef int i2c_port_t;
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1

extern "C" {
    esp_err_t i2c_driver_install(i2c_port_t port, int mode, int slv_rx_buf_len, 
                                 int slv_tx_buf_len, int intr_alloc_flags) {
        return ESP_OK;
    }
    
    esp_err_t i2c_param_config(i2c_port_t port, const void* conf) {
        return ESP_OK;
    }
    
    esp_err_t i2c_master_write_to_device(i2c_port_t port, uint8_t device_address,
                                         const uint8_t* write_buffer, size_t write_size,
                                         int timeout_ms) {
        return ESP_OK;
    }
    
    esp_err_t i2c_master_read_from_device(i2c_port_t port, uint8_t device_address,
                                          uint8_t* read_buffer, size_t read_size,
                                          int timeout_ms) {
        // Mock SHT31 response: temp=25°C, humidity=60%
        if (read_size == 6) {
            // Temperature MSB, LSB, CRC
            read_buffer[0] = 0x64;
            read_buffer[1] = 0x00;
            read_buffer[2] = 0xE6; // CRC
            // Humidity MSB, LSB, CRC
            read_buffer[3] = 0x60;
            read_buffer[4] = 0x00;
            read_buffer[5] = 0x3C; // CRC
        }
        return ESP_OK;
    }
    
    esp_err_t i2c_driver_delete(i2c_port_t port) {
        return ESP_OK;
    }
}

#else
    #include "esp_log.h"
    #include "esp_timer.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/i2c.h"
#endif

// Include C++ sensor classes
#include "ISensor.hpp"
#include "SHT31Sensor.hpp"
#include "I2CDriver.hpp"

// Include implementation files for native testing
// (Not ideal, but necessary for PlatformIO native testing with complex directory structure)
#include "../src/Drivers/Src/I2CDriver.cpp"
#include "../src/HAL/Sensor/Src/SHT31Sensor.cpp"
#include "../src/HAL/Sensor/Src/SensorFactory.cpp"

// ============================================================================
// SENSOR STATUS ENUM TESTS
// ============================================================================

void test_sensor_status_enum_values() {
    TEST_ASSERT_EQUAL(0, static_cast<int>(SensorStatus::OK));
    TEST_ASSERT_NOT_EQUAL(0, static_cast<int>(SensorStatus::ERROR_INIT));
    TEST_ASSERT_NOT_EQUAL(0, static_cast<int>(SensorStatus::ERROR_COMM));
}

// ============================================================================
// SENSOR DATA STRUCTURE TESTS
// ============================================================================

void test_sensor_data_structure() {
    SensorData data;
    data.temperature_celsius = 25.5f;
    data.humidity_percent = 60.0f;
    data.timestamp = 12345;
    
    TEST_ASSERT_EQUAL_FLOAT(25.5f, data.temperature_celsius);
    TEST_ASSERT_EQUAL_FLOAT(60.0f, data.humidity_percent);
    TEST_ASSERT_EQUAL_UINT32(12345, data.timestamp);
}

// ============================================================================
// SENSOR FACTORY TESTS
// ============================================================================

void test_sensor_factory_create_sht31() {
    auto sensor = SensorFactory::create("SHT31");
    TEST_ASSERT_NOT_NULL(sensor.get());
}

void test_sensor_factory_create_unknown_returns_null() {
    auto sensor = SensorFactory::create("UNKNOWN_SENSOR");
    TEST_ASSERT_NULL(sensor.get());
}

void test_sensor_factory_get_available_sensors() {
    auto sensors = SensorFactory::getAvailableSensors();
    TEST_ASSERT_GREATER_THAN(0, sensors.size());
    TEST_ASSERT_TRUE(sensors.size() > 0);
}

// ============================================================================
// I2C DRIVER TESTS
// ============================================================================

void test_i2c_driver_singleton() {
    I2CDriver& driver1 = I2CDriver::getInstance();
    I2CDriver& driver2 = I2CDriver::getInstance();
    
    // Should be the same instance
    TEST_ASSERT_EQUAL_PTR(&driver1, &driver2);
}

void test_i2c_driver_init() {
    I2CConfig config;
    config.sda_pin = 8;
    config.scl_pin = 9;
    config.frequency_hz = 100000;
    
    I2CStatus status = I2CDriver::getInstance().init(config);
    TEST_ASSERT_EQUAL(static_cast<int>(I2CStatus::OK), static_cast<int>(status));
}

// ============================================================================
// SHT31 SENSOR TESTS
// ============================================================================

void test_sht31_sensor_info() {
    auto sensor = SensorFactory::create("SHT31");
    TEST_ASSERT_NOT_NULL(sensor.get());
    
    const SensorInfo& info = sensor->getInfo();
    TEST_ASSERT_EQUAL_STRING("SHT31", info.name.c_str());
    TEST_ASSERT_EQUAL_STRING("Sensirion", info.manufacturer.c_str());
}

void test_sht31_sensor_init() {
    // Initialize I2C first
    I2CConfig i2c_config;
    i2c_config.sda_pin = 8;
    i2c_config.scl_pin = 9;
    i2c_config.frequency_hz = 100000;
    I2CDriver::getInstance().init(i2c_config);
    
    auto sensor = SensorFactory::create("SHT31");
    TEST_ASSERT_NOT_NULL(sensor.get());
    
    SensorStatus status = sensor->init();
    TEST_ASSERT_EQUAL(static_cast<int>(SensorStatus::OK), static_cast<int>(status));
}

void test_sht31_sensor_read_data() {
    // Initialize I2C and sensor
    I2CConfig i2c_config;
    i2c_config.sda_pin = 8;
    i2c_config.scl_pin = 9;
    i2c_config.frequency_hz = 100000;
    I2CDriver::getInstance().init(i2c_config);
    
    auto sensor = SensorFactory::create("SHT31");
    sensor->init();
    
    // Trigger measurement
    SensorStatus status = sensor->triggerMeasurement();
    TEST_ASSERT_EQUAL(static_cast<int>(SensorStatus::OK), static_cast<int>(status));
    
    // Read data
    SensorData data;
    status = sensor->read(data);
    TEST_ASSERT_EQUAL(static_cast<int>(SensorStatus::OK), static_cast<int>(status));
    
    // Verify data is in reasonable range (mocked values)
    TEST_ASSERT_GREATER_THAN(-40.0f, data.temperature_celsius);
    TEST_ASSERT_LESS_THAN(125.0f, data.temperature_celsius);
    TEST_ASSERT_GREATER_THAN(0.0f, data.humidity_percent);
    TEST_ASSERT_LESS_THAN(100.0f, data.humidity_percent);
}

// ============================================================================
// SENSOR CONFIG TESTS
// ============================================================================

void test_sensor_config_structure() {
    SensorConfig config;
    // SensorConfig is used for sensor-specific configuration
    // Test that we can create and use it
    TEST_ASSERT_TRUE(true); // Placeholder test
}

void test_sensor_configure() {
    auto sensor = SensorFactory::create("SHT31");
    TEST_ASSERT_NOT_NULL(sensor.get());
    
    sensor->init();
    
    SensorConfig config;
    // Configure sensor (implementation specific)
    
    SensorStatus status = sensor->configure(config);
    TEST_ASSERT_EQUAL(static_cast<int>(SensorStatus::OK), static_cast<int>(status));
}

// ============================================================================
// TEST RUNNER
// ============================================================================

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // Enum and structure tests
    RUN_TEST(test_sensor_status_enum_values);
    RUN_TEST(test_sensor_data_structure);
    
    // Factory tests
    RUN_TEST(test_sensor_factory_create_sht31);
    RUN_TEST(test_sensor_factory_create_unknown_returns_null);
    RUN_TEST(test_sensor_factory_get_available_sensors);
    
    // I2C driver tests
    RUN_TEST(test_i2c_driver_singleton);
    RUN_TEST(test_i2c_driver_init);
    
    // SHT31 sensor tests
    RUN_TEST(test_sht31_sensor_info);
    RUN_TEST(test_sht31_sensor_init);
    RUN_TEST(test_sht31_sensor_read_data);
    
    // Configuration tests
    RUN_TEST(test_sensor_config_structure);
    RUN_TEST(test_sensor_configure);
    
    return UNITY_END();
}

