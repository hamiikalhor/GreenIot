/**
 * @file StateMachine.cpp
 * @brief State machine implementation
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#include "StateMachine.hpp"
#include "I2CDriver.hpp"
#include "PowerManager.hpp"
#include "BLEMeshManager.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "STATE_MACHINE";

StateMachine::StateMachine()
    : m_current_state(SystemState::INIT)
    , m_previous_state(SystemState::INIT)
    , m_last_measurement_time(0)
    , m_last_transmission_time(0)
    , m_retry_count(0)
{
    m_last_reading = {};
}

void StateMachine::init(const SystemConfig& config) {
    m_config = config;
    ESP_LOGI(TAG, "StateMachine initializing...");
    ESP_LOGI(TAG, "  Measurement interval: %d sec", (int)config.measurement_interval_sec);
    ESP_LOGI(TAG, "  Transmission interval: %d sec", (int)config.transmission_interval_sec);
    ESP_LOGI(TAG, "  Sensor type: %s", config.sensor_type);
    
    m_current_state = SystemState::INIT;
}

void StateMachine::run() {
    switch (m_current_state) {
        case SystemState::INIT:
            handleInit();
            break;
        case SystemState::IDLE:
            handleIdle();
            break;
        case SystemState::MEASURE:
            handleMeasure();
            break;
        case SystemState::TRANSMIT:
            handleTransmit();
            break;
        case SystemState::SLEEP:
            handleSleep();
            break;
        case SystemState::ERROR:
            handleError();
            break;
    }
}

void StateMachine::handleInit() {
    ESP_LOGI(TAG, "STATE: INIT");
    
    // Check wake-up cause
    WakeupSource wakeup_cause = PowerManager::getInstance().getWakeupCause();
    ESP_LOGI(TAG, "Wake-up cause: %s", 
             wakeup_cause == WakeupSource::TIMER ? "Timer" :
             wakeup_cause == WakeupSource::BUTTON ? "Button" :
             wakeup_cause == WakeupSource::POWER_ON ? "Power On" : "Unknown");
    
    // Initialize I2C
    I2CConfig i2c_config;
    i2c_config.sda_pin = 8;
    i2c_config.scl_pin = 9;
    i2c_config.frequency_hz = 100000;
    
    if (I2CDriver::getInstance().init(i2c_config) != I2CStatus::OK) {
        ESP_LOGE(TAG, "I2C init failed");
        transitionTo(SystemState::ERROR);
        return;
    }
    
    // Initialize Power Manager
    PowerConfig power_config;
    power_config.deep_sleep_duration_sec = m_config.measurement_interval_sec;  // Sleep between measurements
    power_config.enable_sensor_power_control = true;
    power_config.sensor_power_pin = 10;  // GPIO 10 for sensor power
    PowerManager::getInstance().init(power_config);
    
    // Turn sensor power on for initialization
    PowerManager::getInstance().sensorPowerOn();
    
    // Initialize BLE Mesh
    BLEMeshConfig mesh_config;
    mesh_config.company_id = 0x02E5;  // Espressif
    mesh_config.product_id = 0x0001;  // GreenIoT Sensor Node
    mesh_config.prov_method = ProvisioningMethod::PB_ADV;
    mesh_config.enable_lpn = true;    // Low power node for battery operation
    
    if (BLEMeshManager::getInstance().init(mesh_config) != BLEMeshStatus::OK) {
        ESP_LOGE(TAG, "BLE Mesh init failed");
        transitionTo(SystemState::ERROR);
        return;
    }
    
    // Enable provisioning if not already provisioned
    if (!BLEMeshManager::getInstance().isProvisioned()) {
        if (BLEMeshManager::getInstance().enableProvisioning() != BLEMeshStatus::OK) {
            ESP_LOGE(TAG, "Failed to enable provisioning");
            transitionTo(SystemState::ERROR);
            return;
        }
    }
    
    // Create sensor
    m_sensor = SensorFactory::create(m_config.sensor_type);
    if (!m_sensor) {
        ESP_LOGE(TAG, "Sensor creation failed: %s", m_config.sensor_type);
        transitionTo(SystemState::ERROR);
        return;
    }
    
    // Initialize sensor
    if (m_sensor->init() != SensorStatus::OK) {
        ESP_LOGE(TAG, "Sensor init failed");
        transitionTo(SystemState::ERROR);
        return;
    }
    
    const SensorInfo& info = m_sensor->getInfo();
    ESP_LOGI(TAG, "Sensor initialized: %s by %s", info.name.c_str(), info.manufacturer.c_str());
    ESP_LOGI(TAG, "  Temp range: %.1f to %.1f °C (±%.2f °C)", 
             info.temp_min, info.temp_max, info.temp_accuracy);
    ESP_LOGI(TAG, "  Humidity range: %.1f to %.1f %% (±%.1f %%)", 
             info.hum_min, info.hum_max, info.hum_accuracy);
    
    // Check battery
    float battery_v = PowerManager::getInstance().getBatteryVoltage();
    uint8_t battery_pct = PowerManager::getInstance().getBatteryPercent();
    ESP_LOGI(TAG, "Battery: %.2fV (%d%%)", battery_v, battery_pct);
    
    m_last_measurement_time = getUptime();
    m_last_transmission_time = getUptime();
    
    transitionTo(SystemState::IDLE);
}

void StateMachine::handleIdle() {
    uint32_t now = getUptime();
    
    // Check if measurement is due
    if ((now - m_last_measurement_time) >= (m_config.measurement_interval_sec * 1000)) {
        transitionTo(SystemState::MEASURE);
        return;
    }
    
    // Else sleep for a bit
    vTaskDelay(pdMS_TO_TICKS(100));
}

void StateMachine::handleMeasure() {
    ESP_LOGI(TAG, "STATE: MEASURE");
    
    // Ensure sensor is powered on
    if (!PowerManager::getInstance().isSensorPowered()) {
        PowerManager::getInstance().sensorPowerOn();
        vTaskDelay(pdMS_TO_TICKS(50));  // Wait for sensor to stabilize
    }
    
    if (!m_sensor) {
        ESP_LOGE(TAG, "Sensor not initialized");
        transitionTo(SystemState::ERROR);
        return;
    }
    
    // Trigger measurement
    if (m_sensor->triggerMeasurement() != SensorStatus::OK) {
        ESP_LOGE(TAG, "Failed to trigger measurement");
        transitionTo(SystemState::ERROR);
        return;
    }
    
    // Read sensor data
    SensorData data;
    SensorStatus status = m_sensor->read(data);
    
    if (status != SensorStatus::OK) {
        ESP_LOGE(TAG, "Sensor read failed: %s", ISensor::statusToString(status));
        m_retry_count++;
        
        if (m_retry_count >= m_config.max_retries) {
            transitionTo(SystemState::ERROR);
        } else {
            vTaskDelay(pdMS_TO_TICKS(1000));
            // Retry measurement
        }
        return;
    }
    
    // Valid data
    m_last_reading = data;
    m_retry_count = 0;
    m_last_measurement_time = getUptime();
    
    ESP_LOGI(TAG, "Measurement successful:");
    ESP_LOGI(TAG, "  Temperature: %.2f °C", data.temperature_celsius);
    ESP_LOGI(TAG, "  Humidity: %.1f %%", data.humidity_percent);
    
    // Check if transmission is due
    uint32_t now = getUptime();
    if ((now - m_last_transmission_time) >= (m_config.transmission_interval_sec * 1000)) {
        transitionTo(SystemState::TRANSMIT);
    } else {
        // After measurement, go to sleep if auto-sleep enabled
        if (PowerManager::getInstance().isAutoSleepEnabled()) {
            transitionTo(SystemState::SLEEP);
        } else {
            transitionTo(SystemState::IDLE);
        }
    }
}

void StateMachine::handleTransmit() {
    ESP_LOGI(TAG, "STATE: TRANSMIT");
    
    // Prepare mesh sensor data
    MeshSensorData mesh_data;
    mesh_data.temperature = m_last_reading.temperature_celsius;
    mesh_data.humidity = m_last_reading.humidity_percent;
    mesh_data.timestamp = m_last_reading.timestamp;
    mesh_data.battery_percent = PowerManager::getInstance().getBatteryPercent();
    
    // Send via BLE Mesh
    BLEMeshStatus status = BLEMeshManager::getInstance().sendSensorData(mesh_data);
    
    if (status != BLEMeshStatus::OK) {
        ESP_LOGW(TAG, "BLE Mesh transmission failed: %s", 
                 BLEMeshManager::statusToString(status));
        
        // If not provisioned, that's OK - we'll try again later
        if (status != BLEMeshStatus::ERROR_NOT_PROVISIONED) {
            m_retry_count++;
            if (m_retry_count >= m_config.max_retries) {
                ESP_LOGE(TAG, "Max transmission retries reached");
                transitionTo(SystemState::ERROR);
                return;
            }
        }
    } else {
        m_retry_count = 0;  // Reset retry counter on success
    }
    
    m_last_transmission_time = getUptime();
    
    // After transmission, enter sleep mode
    transitionTo(SystemState::SLEEP);
}

void StateMachine::handleSleep() {
    ESP_LOGI(TAG, "STATE: SLEEP");
    
    // Calculate sleep duration based on measurement interval
    uint32_t sleep_duration_sec = m_config.measurement_interval_sec;
    
    // Update power statistics before sleep
    uint32_t now = getUptime();
    uint32_t active_time = now - m_last_measurement_time;
    PowerManager::getInstance().updatePowerStats(active_time, sleep_duration_sec * 1000);
    
    // Log power statistics
    PowerStats stats = PowerManager::getInstance().getPowerStats();
    ESP_LOGI(TAG, "Power Statistics:");
    ESP_LOGI(TAG, "  Average current: %.2f µA", stats.avg_current_ua);
    ESP_LOGI(TAG, "  Active current: %.2f mA", stats.active_current_ma);
    ESP_LOGI(TAG, "  Sleep current: %.2f µA", stats.sleep_current_ua);
    ESP_LOGI(TAG, "  Wake-up count: %u", (unsigned int)stats.wakeup_count);
    ESP_LOGI(TAG, "  Estimated battery life: %.1f days", stats.estimated_battery_life_days);
    
    // Enter deep sleep (device will reset on wake-up)
    ESP_LOGI(TAG, "Entering deep sleep for %d seconds...", (int)sleep_duration_sec);
    PowerManager::getInstance().enterDeepSleep(sleep_duration_sec);
    
    // NOTE: Execution NEVER reaches here (device resets on wakeup)
}

void StateMachine::handleError() {
    ESP_LOGE(TAG, "STATE: ERROR");
    
    // Log error details
    float battery_v = PowerManager::getInstance().getBatteryVoltage();
    ESP_LOGE(TAG, "System error occurred. Battery: %.2fV", battery_v);
    
    // Attempt recovery
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    // Reset retry counter
    m_retry_count = 0;
    
    // Try reinitializing
    if (m_sensor) {
        m_sensor->reset();
    }
    
    transitionTo(SystemState::IDLE);
}

void StateMachine::transitionTo(SystemState new_state) {
    if (new_state != m_current_state) {
        ESP_LOGD(TAG, "State transition: %d -> %d", 
                 static_cast<int>(m_current_state), static_cast<int>(new_state));
        m_previous_state = m_current_state;
        m_current_state = new_state;
    }
}

uint32_t StateMachine::getUptime() const {
    return static_cast<uint32_t>(esp_timer_get_time() / 1000);  // milliseconds
}

