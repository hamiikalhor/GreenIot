/**
 * @file test_sensor_simple.cpp
 * @brief Simplified Unit Tests for C++ Sensor Interface
 * 
 * This test file contains mock implementations to run tests natively
 * without needing ESP-IDF or hardware dependencies.
 */

#include <unity.h>
#include <memory>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>

// =======================================================================================
// MOCK ENUMS AND STRUCTURES (mirroring the real ones)
// =======================================================================================

enum class SensorStatus {
    OK = 0,
    ERROR_INIT,
    ERROR_COMM,
    ERROR_TIMEOUT,
    ERROR_CRC,
    ERROR_INVALID_PARAM
};

enum class I2CStatus {
    OK = 0,
    ERROR_INIT,
    ERROR_COMM
};

struct SensorData {
    float temperature_celsius;
    float humidity_percent;
    uint32_t timestamp;
};

struct SensorInfo {
    std::string name;
    std::string manufacturer;
    float temp_min, temp_max, temp_accuracy;
    float hum_min, hum_max, hum_accuracy;
};

struct SensorConfig {};
struct I2CConfig { int sda_pin, scl_pin, frequency_hz; };

// =======================================================================================
// MOCK SENSOR INTERFACE
// =======================================================================================

class ISensor {
public:
    virtual ~ISensor() = default;
    virtual SensorStatus init() = 0;
    virtual SensorStatus read(SensorData& data) = 0;
    virtual SensorStatus triggerMeasurement() = 0;
    virtual SensorStatus configure(const SensorConfig& config) = 0;
    virtual const SensorInfo& getInfo() const = 0;
};

// =======================================================================================
// MOCK I2C DRIVER
// =======================================================================================

class I2CDriver {
public:
    static I2CDriver& getInstance() {
        static I2CDriver instance;
        return instance;
    }
    
    I2CStatus init(const I2CConfig& config) {
        (void)config;
        return I2CStatus::OK;
    }
private:
    I2CDriver() = default;
};

// =======================================================================================
// MOCK SHT31 SENSOR
// =======================================================================================

class SHT31Sensor : public ISensor {
public:
    SHT31Sensor() {
        m_info.name = "SHT31";
        m_info.manufacturer = "Sensirion";
        m_info.temp_min = -40.0f;
        m_info.temp_max = 125.0f;
        m_info.temp_accuracy = 0.3f;
        m_info.hum_min = 0.0f;
        m_info.hum_max = 100.0f;
        m_info.hum_accuracy = 2.0f;
    }
    
    SensorStatus init() override {
        m_initialized = true;
        return SensorStatus::OK;
    }
    
    SensorStatus triggerMeasurement() override {
        if (!m_initialized) return SensorStatus::ERROR_INIT;
        return SensorStatus::OK;
    }
    
    SensorStatus read(SensorData& data) override {
        if (!m_initialized) return SensorStatus::ERROR_INIT;
        
        // Return mock data
        data.temperature_celsius = 25.0f;
        data.humidity_percent = 60.0f;
        data.timestamp = 1000;
        
        return SensorStatus::OK;
    }
    
    SensorStatus configure(const SensorConfig& config) override {
        (void)config;
        return SensorStatus::OK;
    }
    
    const SensorInfo& getInfo() const override {
        return m_info;
    }
    
private:
    bool m_initialized = false;
    SensorInfo m_info;
};

// =======================================================================================
// MOCK SENSOR FACTORY
// =======================================================================================

class SensorFactory {
public:
    static std::unique_ptr<ISensor> create(const std::string& name) {
        if (name == "SHT31") {
            return std::make_unique<SHT31Sensor>();
        }
        return nullptr;
    }
    
    static std::vector<std::string> getAvailableSensors() {
        return {"SHT31"};
    }
};

// =======================================================================================
// UNIT TESTS
// =======================================================================================

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
    TEST_ASSERT_EQUAL(1, sensors.size());
    TEST_ASSERT_EQUAL_STRING("SHT31", sensors[0].c_str());
}

void test_i2c_driver_singleton() {
    I2CDriver& driver1 = I2CDriver::getInstance();
    I2CDriver& driver2 = I2CDriver::getInstance();
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

void test_sht31_sensor_info() {
    auto sensor = SensorFactory::create("SHT31");
    TEST_ASSERT_NOT_NULL(sensor.get());
    
    const SensorInfo& info = sensor->getInfo();
    TEST_ASSERT_EQUAL_STRING("SHT31", info.name.c_str());
    TEST_ASSERT_EQUAL_STRING("Sensirion", info.manufacturer.c_str());
    TEST_ASSERT_EQUAL_FLOAT(-40.0f, info.temp_min);
    TEST_ASSERT_EQUAL_FLOAT(125.0f, info.temp_max);
}

void test_sht31_sensor_init() {
    auto sensor = SensorFactory::create("SHT31");
    TEST_ASSERT_NOT_NULL(sensor.get());
    
    SensorStatus status = sensor->init();
    TEST_ASSERT_EQUAL(static_cast<int>(SensorStatus::OK), static_cast<int>(status));
}

void test_sht31_sensor_read_data() {
    auto sensor = SensorFactory::create("SHT31");
    sensor->init();
    
    // Trigger measurement
    SensorStatus status = sensor->triggerMeasurement();
    TEST_ASSERT_EQUAL(static_cast<int>(SensorStatus::OK), static_cast<int>(status));
    
    // Read data
    SensorData data;
    status = sensor->read(data);
    TEST_ASSERT_EQUAL(static_cast<int>(SensorStatus::OK), static_cast<int>(status));
    
    // Verify mock data
    TEST_ASSERT_EQUAL_FLOAT(25.0f, data.temperature_celsius);
    TEST_ASSERT_EQUAL_FLOAT(60.0f, data.humidity_percent);
    TEST_ASSERT_EQUAL_UINT32(1000, data.timestamp);
}

void test_sht31_sensor_read_without_init_fails() {
    auto sensor = SensorFactory::create("SHT31");
    
    SensorData data;
    SensorStatus status = sensor->read(data);
    
    TEST_ASSERT_EQUAL(static_cast<int>(SensorStatus::ERROR_INIT), static_cast<int>(status));
}

void test_sensor_configure() {
    auto sensor = SensorFactory::create("SHT31");
    sensor->init();
    
    SensorConfig config;
    SensorStatus status = sensor->configure(config);
    TEST_ASSERT_EQUAL(static_cast<int>(SensorStatus::OK), static_cast<int>(status));
}

// =======================================================================================
// TEST RUNNER
// =======================================================================================

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_sensor_factory_create_sht31);
    RUN_TEST(test_sensor_factory_create_unknown_returns_null);
    RUN_TEST(test_sensor_factory_get_available_sensors);
    
    RUN_TEST(test_i2c_driver_singleton);
    RUN_TEST(test_i2c_driver_init);
    
    RUN_TEST(test_sht31_sensor_info);
    RUN_TEST(test_sht31_sensor_init);
    RUN_TEST(test_sht31_sensor_read_data);
    RUN_TEST(test_sht31_sensor_read_without_init_fails);
    RUN_TEST(test_sensor_configure);
    
    return UNITY_END();
}

