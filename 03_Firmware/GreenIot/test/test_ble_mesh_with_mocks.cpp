/**
 * @file test_ble_mesh_with_mocks.cpp
 * @brief BLE Mesh Unit Tests with Embedded Mocks
 * 
 * Native (PC-based) tests with mock BLE Mesh implementation included.
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 */

#include <unity.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

// Arduino stubs
static inline uint32_t millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

static inline void delay(uint32_t ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

// Include BLE Mesh interface
#include "HAL/Wireless/ble_mesh_interface.h"
#include "HAL/Wireless/ble_mesh_config.h"

// ============================================================================
// MOCK IMPLEMENTATION (Embedded in test file)
// ============================================================================

static bool g_mock_initialized = false;
static bool g_mock_provisioned = false;
static bool g_mock_provisioning_enabled = false;
static ble_mesh_config_t g_mock_config;
static ble_mesh_status_t g_mock_status;
static mesh_sensor_data_t g_mock_last_published_data;
static uint32_t g_mock_publish_count = 0;

ble_mesh_err_t ble_mesh_init(const ble_mesh_config_t *config) {
    if (config == NULL) return BLE_MESH_ERROR_INVALID_PARAM;
    if (g_mock_initialized) return BLE_MESH_ERROR_ALREADY_INITIALIZED;
    
    memcpy(&g_mock_config, config, sizeof(ble_mesh_config_t));
    
    g_mock_status.initialized = true;
    g_mock_status.provisioned = false;
    g_mock_status.unicast_addr = 0x0000;
    g_mock_status.net_idx = 0x0000;
    g_mock_status.app_idx = 0x0000;
    g_mock_status.prov_state = BLE_MESH_PROV_STATE_IDLE;
    g_mock_status.features = config->features;
    g_mock_status.uptime_ms = millis();
    
    g_mock_initialized = true;
    g_mock_provisioned = false;
    g_mock_provisioning_enabled = false;
    g_mock_publish_count = 0;
    
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_deinit(void) {
    if (!g_mock_initialized) return BLE_MESH_ERROR_NOT_INITIALIZED;
    g_mock_initialized = false;
    g_mock_provisioned = false;
    g_mock_provisioning_enabled = false;
    g_mock_status.initialized = false;
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_enable_provisioning(void) {
    if (!g_mock_initialized) return BLE_MESH_ERROR_NOT_INITIALIZED;
    if (g_mock_provisioned) return BLE_MESH_ERROR_ALREADY_PROVISIONED;
    g_mock_provisioning_enabled = true;
    g_mock_status.prov_state = BLE_MESH_PROV_STATE_LINK_OPEN;
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_disable_provisioning(void) {
    if (!g_mock_initialized) return BLE_MESH_ERROR_NOT_INITIALIZED;
    g_mock_provisioning_enabled = false;
    g_mock_status.prov_state = BLE_MESH_PROV_STATE_IDLE;
    return BLE_MESH_OK;
}

bool ble_mesh_is_provisioned(void) {
    return g_mock_provisioned;
}

ble_mesh_err_t ble_mesh_publish_sensor_data(const mesh_sensor_data_t *data) {
    if (!g_mock_initialized) return BLE_MESH_ERROR_NOT_INITIALIZED;
    if (data == NULL) return BLE_MESH_ERROR_INVALID_PARAM;
    memcpy(&g_mock_last_published_data, data, sizeof(mesh_sensor_data_t));
    g_mock_publish_count++;
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_get_status(ble_mesh_status_t *status) {
    if (!g_mock_initialized) return BLE_MESH_ERROR_NOT_INITIALIZED;
    if (status == NULL) return BLE_MESH_ERROR_INVALID_PARAM;
    g_mock_status.uptime_ms = millis() - g_mock_status.uptime_ms;
    memcpy(status, &g_mock_status, sizeof(ble_mesh_status_t));
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_register_prov_callback(ble_mesh_prov_callback_t callback) {
    if (!g_mock_initialized) return BLE_MESH_ERROR_NOT_INITIALIZED;
    (void)callback;
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_register_sensor_callback(ble_mesh_sensor_callback_t callback) {
    if (!g_mock_initialized) return BLE_MESH_ERROR_NOT_INITIALIZED;
    (void)callback;
    return BLE_MESH_OK;
}

ble_mesh_err_t ble_mesh_factory_reset(void) {
    if (!g_mock_initialized) return BLE_MESH_ERROR_NOT_INITIALIZED;
    g_mock_provisioned = false;
    g_mock_status.provisioned = false;
    g_mock_status.unicast_addr = 0x0000;
    g_mock_status.net_idx = 0x0000;
    g_mock_status.app_idx = 0x0000;
    g_mock_status.prov_state = BLE_MESH_PROV_STATE_IDLE;
    return BLE_MESH_OK;
}

const char* ble_mesh_status_to_string(ble_mesh_err_t status) {
    switch (status) {
        case BLE_MESH_OK: return "OK";
        case BLE_MESH_ERROR_INVALID_PARAM: return "Invalid parameter";
        case BLE_MESH_ERROR_NOT_INITIALIZED: return "Not initialized";
        case BLE_MESH_ERROR_ALREADY_INITIALIZED: return "Already initialized";
        case BLE_MESH_ERROR_NO_MEMORY: return "No memory";
        case BLE_MESH_ERROR_TIMEOUT: return "Timeout";
        case BLE_MESH_ERROR_NOT_PROVISIONED: return "Not provisioned";
        case BLE_MESH_ERROR_ALREADY_PROVISIONED: return "Already provisioned";
        case BLE_MESH_ERROR_PUBLISH_FAILED: return "Publish failed";
        default: return "Unknown error";
    }
}

const char* ble_mesh_prov_state_to_string(ble_mesh_prov_state_t state) {
    switch (state) {
        case BLE_MESH_PROV_STATE_IDLE: return "Idle";
        case BLE_MESH_PROV_STATE_LINK_OPEN: return "Link Open";
        case BLE_MESH_PROV_STATE_LINK_ACTIVE: return "Link Active";
        case BLE_MESH_PROV_STATE_SUCCESS: return "Success";
        case BLE_MESH_PROV_STATE_FAILED: return "Failed";
        default: return "Unknown";
    }
}

void mock_reset() {
    g_mock_initialized = false;
    g_mock_provisioned = false;
    g_mock_provisioning_enabled = false;
    g_mock_publish_count = 0;
    memset(&g_mock_config, 0, sizeof(ble_mesh_config_t));
    memset(&g_mock_status, 0, sizeof(ble_mesh_status_t));
    memset(&g_mock_last_published_data, 0, sizeof(mesh_sensor_data_t));
}

// ============================================================================
// TESTS (Same as before)
// ============================================================================

static ble_mesh_config_t test_config;
static mesh_sensor_data_t test_sensor_data;

void setUp(void) {
    memset(&test_config, 0, sizeof(ble_mesh_config_t));
    strncpy(test_config.device_name, "Test-Node", BLE_MESH_DEV_NAME_MAX_LEN);
    test_config.company_id = BLE_MESH_COMPANY_ID_ESPRESSIF;
    test_config.product_id = 0x0001;
    test_config.version_id = 0x0001;
    test_config.enable_pb_adv = true;
    test_config.enable_pb_gatt = true;
    test_config.enable_lpn = true;
    test_config.lpn_poll_interval_ms = 10000;
    test_config.features = BLE_MESH_FEATURE_LOW_POWER | BLE_MESH_FEATURE_PROXY;
    
    test_sensor_data.temperature_celsius = 22.5f;
    test_sensor_data.humidity_percent = 65.0f;
    test_sensor_data.battery_level = 95;
    test_sensor_data.timestamp = 123456;
}

void tearDown(void) {
    if (ble_mesh_is_provisioned()) {
        ble_mesh_deinit();
    }
    mock_reset();
}

// Configuration Tests
void test_ble_mesh_config_intervals(void) {
    TEST_ASSERT_EQUAL_UINT32(300000, BLE_MESH_PUBLISH_INTERVAL_MS);
    TEST_ASSERT_EQUAL_UINT32(300, BLE_MESH_PUBLISH_INTERVAL_SEC);
    TEST_ASSERT_EQUAL_UINT32(10000, BLE_MESH_LPN_POLL_INTERVAL_MS);
    TEST_ASSERT_EQUAL_UINT8(7, BLE_MESH_DEFAULT_TTL);
}

void test_ble_mesh_property_ids(void) {
    TEST_ASSERT_EQUAL_HEX16(0x004F, BLE_MESH_PROP_ID_TEMPERATURE);
    TEST_ASSERT_EQUAL_HEX16(0x0076, BLE_MESH_PROP_ID_HUMIDITY);
    TEST_ASSERT_EQUAL_HEX16(0x006E, BLE_MESH_PROP_ID_BATTERY_LEVEL);
}

// Initialization Tests
void test_ble_mesh_init_with_valid_config(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
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
    
    status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_ERROR_ALREADY_INITIALIZED, status);
    
    ble_mesh_deinit();
}

// Provisioning Tests
void test_ble_mesh_provisioning_enable(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_enable_provisioning();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    ble_mesh_deinit();
}

void test_ble_mesh_is_provisioned_initially_false(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    TEST_ASSERT_FALSE(ble_mesh_is_provisioned());
    
    ble_mesh_deinit();
}

// Publishing Tests  
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

// Status Tests
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

// String Tests
void test_ble_mesh_status_to_string(void) {
    TEST_ASSERT_EQUAL_STRING("OK", ble_mesh_status_to_string(BLE_MESH_OK));
    TEST_ASSERT_EQUAL_STRING("Invalid parameter", 
                             ble_mesh_status_to_string(BLE_MESH_ERROR_INVALID_PARAM));
    TEST_ASSERT_EQUAL_STRING("Not initialized", 
                             ble_mesh_status_to_string(BLE_MESH_ERROR_NOT_INITIALIZED));
}

void test_ble_mesh_prov_state_to_string(void) {
    TEST_ASSERT_EQUAL_STRING("Idle", 
                             ble_mesh_prov_state_to_string(BLE_MESH_PROV_STATE_IDLE));
    TEST_ASSERT_EQUAL_STRING("Link Open", 
                             ble_mesh_prov_state_to_string(BLE_MESH_PROV_STATE_LINK_OPEN));
}

// Factory Reset Test
void test_ble_mesh_factory_reset(void) {
    ble_mesh_err_t status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_factory_reset();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    TEST_ASSERT_FALSE(ble_mesh_is_provisioned());
    
    ble_mesh_deinit();
}

// Integration Test
void test_ble_mesh_full_workflow(void) {
    ble_mesh_err_t status;
    
    status = ble_mesh_init(&test_config);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_enable_provisioning();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_publish_sensor_data(&test_sensor_data);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    ble_mesh_status_t mesh_status;
    status = ble_mesh_get_status(&mesh_status);
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    TEST_ASSERT_TRUE(mesh_status.initialized);
    
    status = ble_mesh_disable_provisioning();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
    
    status = ble_mesh_deinit();
    TEST_ASSERT_EQUAL(BLE_MESH_OK, status);
}

// Main Test Runner
int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_ble_mesh_config_intervals);
    RUN_TEST(test_ble_mesh_property_ids);
    RUN_TEST(test_ble_mesh_init_with_valid_config);
    RUN_TEST(test_ble_mesh_init_with_null_config);
    RUN_TEST(test_ble_mesh_double_init);
    RUN_TEST(test_ble_mesh_provisioning_enable);
    RUN_TEST(test_ble_mesh_is_provisioned_initially_false);
    RUN_TEST(test_ble_mesh_publish_sensor_data_valid);
    RUN_TEST(test_ble_mesh_publish_sensor_data_null);
    RUN_TEST(test_ble_mesh_publish_without_init);
    RUN_TEST(test_ble_mesh_get_status_valid);
    RUN_TEST(test_ble_mesh_status_to_string);
    RUN_TEST(test_ble_mesh_prov_state_to_string);
    RUN_TEST(test_ble_mesh_factory_reset);
    RUN_TEST(test_ble_mesh_full_workflow);
    
    return UNITY_END();
}

