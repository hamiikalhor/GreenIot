/**
 * @file test_sensor_interface.cpp
 * @brief Native Unit Tests for Sensor Interface (HAL Layer)
 * 
 * These tests run on PC (native) with mocked dependencies.
 */

#include <unity.h>
#include <string.h>

// Mock implementations for native build
#ifdef NATIVE_BUILD

// Mock Arduino delay
extern "C" void delay(unsigned long ms) {
    // No-op for native tests
}

// Mock millis
extern "C" unsigned long millis(void) {
    return 1000;
}

#endif

// Include sensor interface
extern "C" {
    #include "HAL/Sensor/sensor_interface.h"
}

// ============================================================================
// SENSOR INTERFACE TESTS
// ============================================================================

void test_sensor_data_is_valid_with_valid_data() {
    sensor_data_t data = {
        .temperature_celsius = 25.0f,
        .humidity_percent = 60.0f,
        .timestamp = 1000,
        .quality_flags = 0xC0  // Both temp and humidity valid
    };
    
    TEST_ASSERT_TRUE(sensor_data_is_valid(&data));
}

void test_sensor_data_is_valid_with_invalid_temp() {
    sensor_data_t data = {
        .temperature_celsius = 25.0f,
        .humidity_percent = 60.0f,
        .timestamp = 1000,
        .quality_flags = 0x40  // Only humidity valid
    };
    
    TEST_ASSERT_FALSE(sensor_data_is_valid(&data));
}

void test_sensor_data_is_valid_with_invalid_hum() {
    sensor_data_t data = {
        .temperature_celsius = 25.0f,
        .humidity_percent = 60.0f,
        .timestamp = 1000,
        .quality_flags = 0x80  // Only temperature valid
    };
    
    TEST_ASSERT_FALSE(sensor_data_is_valid(&data));
}

void test_sensor_data_is_valid_with_null_pointer() {
    TEST_ASSERT_FALSE(sensor_data_is_valid(NULL));
}

void test_sensor_status_to_string_conversions() {
    TEST_ASSERT_EQUAL_STRING("OK", sensor_status_to_string(SENSOR_OK));
    TEST_ASSERT_EQUAL_STRING("Initialization Failed", sensor_status_to_string(SENSOR_ERROR_INIT));
    TEST_ASSERT_EQUAL_STRING("Communication Error", sensor_status_to_string(SENSOR_ERROR_COMM));
    TEST_ASSERT_EQUAL_STRING("Timeout", sensor_status_to_string(SENSOR_ERROR_TIMEOUT));
    TEST_ASSERT_EQUAL_STRING("CRC Mismatch", sensor_status_to_string(SENSOR_ERROR_CRC));
}

void test_sensor_get_interface_returns_valid_for_known_sensor() {
    const sensor_interface_t *sensor = sensor_get_interface("SHT31");
    TEST_ASSERT_NOT_NULL(sensor);
    TEST_ASSERT_NOT_NULL(sensor->init);
    TEST_ASSERT_NOT_NULL(sensor->read);
    TEST_ASSERT_NOT_NULL(sensor->trigger_measurement);
}

void test_sensor_get_interface_returns_null_for_unknown_sensor() {
    const sensor_interface_t *sensor = sensor_get_interface("UNKNOWN");
    TEST_ASSERT_NULL(sensor);
}

void test_sensor_get_available_sensors_returns_list() {
    uint8_t count = 0;
    const char **sensors = sensor_get_available_sensors(&count);
    
    TEST_ASSERT_NOT_NULL(sensors);
    TEST_ASSERT_GREATER_THAN(0, count);
    TEST_ASSERT_NOT_NULL(sensors[0]);
}

// ============================================================================
// TEST RUNNER
// ============================================================================

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_sensor_data_is_valid_with_valid_data);
    RUN_TEST(test_sensor_data_is_valid_with_invalid_temp);
    RUN_TEST(test_sensor_data_is_valid_with_invalid_hum);
    RUN_TEST(test_sensor_data_is_valid_with_null_pointer);
    RUN_TEST(test_sensor_status_to_string_conversions);
    RUN_TEST(test_sensor_get_interface_returns_valid_for_known_sensor);
    RUN_TEST(test_sensor_get_interface_returns_null_for_unknown_sensor);
    RUN_TEST(test_sensor_get_available_sensors_returns_list);
    
    return UNITY_END();
}

