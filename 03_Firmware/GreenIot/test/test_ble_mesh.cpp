/**
 * @file test_ble_mesh.cpp
 * @brief Unit Tests for BLE Mesh Sensor Server
 * 
 * Tests for BLE Mesh functionality including:
 * - Stack initialization
 * - Sensor data publishing
 * - Property encoding/decoding
 * - Provisioning status
 * - Adaptive publishing logic
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 */

#include <unity.h>

#ifdef NATIVE_BUILD
#include "Arduino_stub.h"
#else
#include <Arduino.h>
#endif

#include "HAL/Wireless/ble_mesh_interface.h"
#include "HAL/Wireless/ble_mesh_config.h"

#ifdef MOCK_BLE_MESH
#include "mock_ble_mesh.h"
#endif

#include <string.h>

// Test data
static ble_mesh_config_t test_config;
static mesh_sensor_data_t test_sensor_data;

// ============================================================================
// Test Setup & Teardown
// ============================================================================

void setUp(void) {
    // Initialize test configuration
    memset(&test_config, 0, sizeof(ble_mesh_config_t));
    
    // Set basic configuration
    strncpy(test_config.device_name, "Test-Node", BLE_MESH_DEV_NAME_MAX_LEN);
    test_config.company_id = BLE_MESH_COMPANY_ID_ESPRESSIF;
    test_config.product_id = 0x0001;
    test_config.version_id = 0x0001;
    test_config.enable_pb_adv = true;
    test_config.enable_pb_gatt = true;
    test_config.enable_lpn = true;
    test_config.lpn_poll_interval_ms = 10000;
    test_config.features = BLE_MESH_FEATURE_LOW_POWER | BLE_MESH_FEATURE_PROXY;
    
    // Initialize test sensor data
    test_sensor_data.temperature_celsius = 22.5f;
    test_sensor_data.humidity_percent = 65.0f;
    test_sensor_data.battery_level = 95;
    test_sensor_data.timestamp = 123456;
}

void tearDown(void) {
    // Cleanup if mesh was initialized
    if (ble_mesh_is_provisioned()) {
        ble_mesh_deinit();
    }
    
#ifdef MOCK_BLE_MESH
    // Reset mock state for next test
    mock_ble_mesh_reset();
#endif
}

// ============================================================================
// Configuration Tests
// ============================================================================

void test_ble_mesh_config_intervals(void) {
    // Test that configured intervals are optimal
    TEST_ASSERT_EQUAL_UINT32(300000, BLE_MESH_PUBLISH_INTERVAL_MS);
    TEST_ASSERT_EQUAL_UINT32(300, BLE_MESH_PUBLISH_INTERVAL_SEC);
    
    // Test LPN configuration
    TEST_ASSERT_EQUAL_UINT32(10000, BLE_MESH_LPN_POLL_INTERVAL_MS);
    TEST_ASSERT_EQUAL_UINT32(300, BLE_MESH_LPN_POLL_TIMEOUT_MS);
    TEST_ASSERT_EQUAL_UINT32(100, BLE_MESH_LPN_RECV_DELAY_MS);
    
    // Test TTL
    TEST_ASSERT_EQUAL_UINT8(7, BLE_MESH_DEFAULT_TTL);
}

void test_ble_mesh_property_ids(void) {
    // Test that property IDs match BLE Mesh specification
    TEST_ASSERT_EQUAL_HEX16(0x004F, BLE_MESH_PROP_ID_TEMPERATURE);
    TEST_ASSERT_EQUAL_HEX16(0x0076, BLE_MESH_PROP_ID_HUMIDITY);
    TEST_ASSERT_EQUAL_HEX16(0x006E, BLE_MESH_PROP_ID_BATTERY_LEVEL);
}

void test_basil_thresholds(void) {
    // Test basil cultivation thresholds
    TEST_ASSERT_EQUAL_FLOAT(15.0f, BASIL_TEMP_MIN_CRITICAL);
    TEST_ASSERT_EQUAL_FLOAT(18.0f, BASIL_TEMP_MIN_OPTIMAL);
    TEST_ASSERT_EQUAL_FLOAT(25.0f, BASIL_TEMP_MAX_OPTIMAL);
    TEST_ASSERT_EQUAL_FLOAT(30.0f, BASIL_TEMP_MAX_CRITICAL);
    
    TEST_ASSERT_EQUAL_FLOAT(40.0f, BASIL_HUM_MIN_CRITICAL);
    TEST_ASSERT_EQUAL_FLOAT(60.0f, BASIL_HUM_MIN_OPTIMAL);
    TEST_ASSERT_EQUAL_FLOAT(70.0f, BASIL_HUM_MAX_OPTIMAL);
    TEST_ASSERT_EQUAL_FLOAT(80.0f, BASIL_HUM_MAX_CRITICAL);
}

// ============================================================================
// Initialization Tests
// ============================================================================

void test_ble_mesh_init_with_valid_config(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // Verify initialization status
    ble_mesh_status_t mesh_status;
    status = ble_mesh_get_status(&mesh_status);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    TEST_ASSERT_TRUE(mesh_status.initialized);
    
    ble_mesh_deinit();
}

void test_ble_mesh_init_with_null_config(void) {
    ble_mesh_err_t status = ble_mesh_init(NULL);
    TEST_ASSERT_EQUAL(BLE_MESH_ERROR_INVALID_PARAM, status);
}

void test_ble_mesh_double_init(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // Try to initialize again
    status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_ERROR_ALREADY_INITIALIZED, status);
    
    ble_mesh_deinit();
}

// ============================================================================
// Provisioning Tests
// ============================================================================

void test_ble_mesh_provisioning_enable(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_enable_provisioning();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    ble_mesh_deinit();
}

void test_ble_mesh_provisioning_disable(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_enable_provisioning();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_disable_provisioning();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    ble_mesh_deinit();
}

void test_ble_mesh_is_provisioned_initially_false(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // Should not be provisioned initially
    TEST_ASSERT_FALSE(ble_mesh_is_provisioned());
    
    ble_mesh_deinit();
}

// ============================================================================
// Sensor Data Publishing Tests
// ============================================================================

void test_ble_mesh_publish_sensor_data_valid(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    ble_mesh_deinit();
}

void test_ble_mesh_publish_sensor_data_null(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_publish_sensor_data(NULL);
    TEST_ASSERT_EQUAL(BLE_MESH_ERROR_INVALID_PARAM, status);
    
    ble_mesh_deinit();
}

void test_ble_mesh_publish_without_init(void) {
    ble_mesh_err_t status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_ERROR_NOT_INITIALIZED, status);
}

void test_ble_mesh_publish_temperature_range(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // Test min temperature
    test_sensor_data.temperature_celsius = -40.0f;
    status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // Test max temperature
    test_sensor_data.temperature_celsius = 85.0f;
    status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // Test optimal basil temperature
    test_sensor_data.temperature_celsius = 22.0f;
    status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    ble_mesh_deinit();
}

void test_ble_mesh_publish_humidity_range(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // Test min humidity
    test_sensor_data.humidity_percent = 0.0f;
    status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // Test max humidity
    test_sensor_data.humidity_percent = 100.0f;
    status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // Test optimal basil humidity
    test_sensor_data.humidity_percent = 65.0f;
    status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    ble_mesh_deinit();
}

// ============================================================================
// Status Tests
// ============================================================================

void test_ble_mesh_get_status_valid(void) {
    ble_mesh_status_t mesh_status;
    
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_get_status(&mesh_status);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    TEST_ASSERT_TRUE(mesh_status.initialized);
    TEST_ASSERT_FALSE(mesh_status.provisioned);
    TEST_ASSERT_EQUAL_UINT16(0x0000, mesh_status.unicast_addr);
    
    ble_mesh_deinit();
}

void test_ble_mesh_get_status_null(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_get_status(NULL);
    TEST_ASSERT_EQUAL(BLE_MESH_ERROR_INVALID_PARAM, status);
    
    ble_mesh_deinit();
}

// ============================================================================
// String Conversion Tests
// ============================================================================

void test_ble_mesh_status_to_string(void) {
    TEST_ASSERT_EQUAL_STRING("OK", ble_mesh_status_to_string(BLE_MESH_OK));
    TEST_ASSERT_EQUAL_STRING("Invalid parameter", 
                             ble_mesh_status_to_string(BLE_MESH_ERROR_INVALID_PARAM));
    TEST_ASSERT_EQUAL_STRING("Not initialized", 
                             ble_mesh_status_to_string(BLE_MESH_ERROR_NOT_INITIALIZED));
    TEST_ASSERT_EQUAL_STRING("Already initialized", 
                             ble_mesh_status_to_string(BLE_MESH_ERROR_ALREADY_INITIALIZED));
    TEST_ASSERT_EQUAL_STRING("Not provisioned", 
                             ble_mesh_status_to_string(BLE_MESH_ERROR_NOT_PROVISIONED));
    TEST_ASSERT_EQUAL_STRING("Already provisioned", 
                             ble_mesh_status_to_string(BLE_MESH_ERROR_ALREADY_PROVISIONED));
}

void test_ble_mesh_prov_state_to_string(void) {
    TEST_ASSERT_EQUAL_STRING("Idle", 
                             ble_mesh_prov_state_to_string(BLE_MESH_PROV_STATE_IDLE));
    TEST_ASSERT_EQUAL_STRING("Link Open", 
                             ble_mesh_prov_state_to_string(BLE_MESH_PROV_STATE_LINK_OPEN));
    TEST_ASSERT_EQUAL_STRING("Link Active", 
                             ble_mesh_prov_state_to_string(BLE_MESH_PROV_STATE_LINK_ACTIVE));
    TEST_ASSERT_EQUAL_STRING("Success", 
                             ble_mesh_prov_state_to_string(BLE_MESH_PROV_STATE_SUCCESS));
    TEST_ASSERT_EQUAL_STRING("Failed", 
                             ble_mesh_prov_state_to_string(BLE_MESH_PROV_STATE_FAILED));
}

// ============================================================================
// Factory Reset Test
// ============================================================================

void test_ble_mesh_factory_reset(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_factory_reset();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // After factory reset, node should not be provisioned
    TEST_ASSERT_FALSE(ble_mesh_is_provisioned());
    
    ble_mesh_deinit();
}

// ============================================================================
// Power Consumption Calculation Tests
// ============================================================================

void test_power_consumption_estimates(void) {
    // Test power consumption constants are defined
    TEST_ASSERT_EQUAL_UINT32(5000, BLE_MESH_POWER_SENSOR_ACTIVE_UA);
    TEST_ASSERT_EQUAL_UINT32(12000, BLE_MESH_POWER_BLE_TX_UA);
    TEST_ASSERT_EQUAL_UINT32(11000, BLE_MESH_POWER_BLE_RX_UA);
    TEST_ASSERT_EQUAL_UINT32(800, BLE_MESH_POWER_LPN_SLEEP_UA);
    TEST_ASSERT_EQUAL_UINT32(10, BLE_MESH_POWER_DEEP_SLEEP_UA);
    
    // Calculate expected battery life (simplified)
    // 5-minute intervals with LPN sleep
    uint32_t cycles_per_day = (24 * 60) / 5;  // 288 cycles
    float energy_per_cycle = 0.085f;  // mAh (from spec)
    float daily_consumption = cycles_per_day * energy_per_cycle;
    uint32_t battery_capacity = 3000;  // mAh
    float battery_life_days = battery_capacity / daily_consumption;
    
    // Should be around 122 days (4 months)
    TEST_ASSERT_FLOAT_WITHIN(10.0f, 122.0f, battery_life_days);
}

// ============================================================================
// Integration Test (Full Workflow)
// ============================================================================

void test_ble_mesh_full_workflow(void) {
    ble_mesh_err_t status;
    
    // 1. Initialize
    status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // 2. Enable provisioning
    status = ble_mesh_enable_provisioning();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // 3. Publish sensor data
    status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // 4. Get status
    ble_mesh_status_t mesh_status;
    status = ble_mesh_get_status(&mesh_status);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    TEST_ASSERT_TRUE(mesh_status.initialized);
    
    // 5. Disable provisioning
    status = ble_mesh_disable_provisioning();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    // 6. Deinitialize
    status = ble_mesh_deinit();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
}

// ============================================================================
// Main Test Runner
// ============================================================================

void setup() {
    delay(2000);  // Service delay for serial monitor
    
    UNITY_BEGIN();
    
    // Configuration tests
    RUN_TEST(test_ble_mesh_config_intervals);
    RUN_TEST(test_ble_mesh_property_ids);
    RUN_TEST(test_basil_thresholds);
    
    // Initialization tests
    RUN_TEST(test_ble_mesh_init_with_valid_config);
    RUN_TEST(test_ble_mesh_init_with_null_config);
    RUN_TEST(test_ble_mesh_double_init);
    
    // Provisioning tests
    RUN_TEST(test_ble_mesh_provisioning_enable);
    RUN_TEST(test_ble_mesh_provisioning_disable);
    RUN_TEST(test_ble_mesh_is_provisioned_initially_false);
    
    // Sensor data publishing tests
    RUN_TEST(test_ble_mesh_publish_sensor_data_valid);
    RUN_TEST(test_ble_mesh_publish_sensor_data_null);
    RUN_TEST(test_ble_mesh_publish_without_init);
    RUN_TEST(test_ble_mesh_publish_temperature_range);
    RUN_TEST(test_ble_mesh_publish_humidity_range);
    
    // Status tests
    RUN_TEST(test_ble_mesh_get_status_valid);
    RUN_TEST(test_ble_mesh_get_status_null);
    
    // String conversion tests
    RUN_TEST(test_ble_mesh_status_to_string);
    RUN_TEST(test_ble_mesh_prov_state_to_string);
    
    // Factory reset
    RUN_TEST(test_ble_mesh_factory_reset);
    
    // Power consumption
    RUN_TEST(test_power_consumption_estimates);
    
    // Integration test
    RUN_TEST(test_ble_mesh_full_workflow);
    
    UNITY_END();
}

void loop() {
    // Nothing to do here
}

