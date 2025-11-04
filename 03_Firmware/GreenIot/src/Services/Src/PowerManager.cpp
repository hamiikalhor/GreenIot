/**
 * @file PowerManager.cpp
 * @brief Power management implementation for ESP32-C3
 * 
 * Implements Issue #4: Deep Sleep & Wake-up Logic
 * - Periodic wake-up timer
 * - GPIO sensor power pin control
 * - Current consumption measurement
 * - RTC memory for state preservation
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 */

#include "PowerManager.hpp"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc.h"

static const char* TAG = "POWER";

// RTC memory for state preservation across deep sleep
RTC_DATA_ATTR static uint32_t s_boot_count = 0;
RTC_DATA_ATTR static uint32_t s_total_wakeups = 0;
RTC_DATA_ATTR static uint32_t s_total_active_time_ms = 0;
RTC_DATA_ATTR static uint32_t s_total_sleep_time_ms = 0;

PowerManager& PowerManager::getInstance() {
    static PowerManager instance;
    return instance;
}

void PowerManager::init(const PowerConfig& config) {
    m_config = config;
    
    // Initialize GPIO for sensor power control
    if (config.enable_sensor_power_control) {
        initGPIO();
        sensorPowerOff();  // Start with sensor off (will be turned on when needed)
    }
    
    // Initialize ADC for battery monitoring
    initADC();
    
    // Restore state from RTC memory (if coming from deep sleep)
    restoreStateFromRTC();
    
    // Increment boot count
    s_boot_count++;
    
    ESP_LOGI(TAG, "PowerManager initialized");
    ESP_LOGI(TAG, "  Boot count: %u", (unsigned int)s_boot_count);
    ESP_LOGI(TAG, "  Deep sleep interval: %d sec", (int)config.deep_sleep_duration_sec);
    ESP_LOGI(TAG, "  Light sleep: %d ms", (int)config.light_sleep_duration_ms);
    ESP_LOGI(TAG, "  Sensor power pin: GPIO %d (%s)", 
             config.sensor_power_pin,
             config.enable_sensor_power_control ? "enabled" : "disabled");
    
    m_initialized = true;
}

void PowerManager::initGPIO() {
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << m_config.sensor_power_pin);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    gpio_config(&io_conf);
    ESP_LOGI(TAG, "Sensor power GPIO %d configured", m_config.sensor_power_pin);
}

void PowerManager::sensorPowerOn() {
    if (!m_config.enable_sensor_power_control) {
        return;  // Power control disabled
    }
    
    gpio_set_level(static_cast<gpio_num_t>(m_config.sensor_power_pin), 1);
    m_sensor_powered = true;
    
    // Wait for sensor to stabilize (typically 10-50ms for SHT31)
    vTaskDelay(pdMS_TO_TICKS(50));
    
    ESP_LOGI(TAG, "Sensor power ON (GPIO %d)", m_config.sensor_power_pin);
}

void PowerManager::sensorPowerOff() {
    if (!m_config.enable_sensor_power_control) {
        return;  // Power control disabled
    }
    
    gpio_set_level(static_cast<gpio_num_t>(m_config.sensor_power_pin), 0);
    m_sensor_powered = false;
    
    ESP_LOGI(TAG, "Sensor power OFF (GPIO %d)", m_config.sensor_power_pin);
}

void PowerManager::configureWakeupTimer(uint32_t duration_sec) {
    m_config.deep_sleep_duration_sec = duration_sec;
    ESP_LOGI(TAG, "Wake-up timer configured: %d seconds", (int)duration_sec);
}

void PowerManager::enterLightSleep(uint32_t duration_ms) {
    ESP_LOGI(TAG, "Entering light sleep for %d ms", (int)duration_ms);
    
    // Turn off sensor to save power
    sensorPowerOff();
    
    // Configure wake-up timer
    esp_sleep_enable_timer_wakeup(duration_ms * 1000ULL);
    
    // Enter light sleep (BLE Mesh connection maintained)
    esp_light_sleep_start();
    
    // Turn sensor back on after wake-up
    sensorPowerOn();
    
    ESP_LOGI(TAG, "Woke from light sleep");
}

void PowerManager::enterDeepSleep(uint32_t duration_sec) {
    ESP_LOGI(TAG, "Preparing for deep sleep (%d seconds)...", (int)duration_sec);
    
    // Save state to RTC memory before sleep
    saveStateToRTC();
    
    // Turn off sensor to save power
    sensorPowerOff();
    
    // Configure wake-up timer
    esp_sleep_enable_timer_wakeup(duration_sec * 1000000ULL);
    
    // Optional: Enable GPIO wake-up (e.g., button press)
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_9, 0);  // Wake on LOW
    
    // Configure which domains to power down in deep sleep
    // Keep RTC peripherals powered for timer wake-up
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_ON);
    
    ESP_LOGI(TAG, "Entering deep sleep...");
    esp_deep_sleep_start();
    
    // NOTE: Execution NEVER reaches here (device resets on wakeup)
}

WakeupSource PowerManager::getWakeupCause() {
    if (s_boot_count == 0) {
        return WakeupSource::POWER_ON;  // First boot
    }
    
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    
    switch (cause) {
        case ESP_SLEEP_WAKEUP_TIMER:
            s_total_wakeups++;
            return WakeupSource::TIMER;
        case ESP_SLEEP_WAKEUP_EXT0:
        case ESP_SLEEP_WAKEUP_EXT1:
            return WakeupSource::BUTTON;
        default:
            return WakeupSource::UNKNOWN;
    }
}

void PowerManager::initADC() {
    // ADC1 config for battery voltage monitoring
    adc1_config_width(ADC_WIDTH_BIT_12);
    
    if (m_config.battery_adc_pin < 10) {
        // ESP32-C3 ADC1 channels: GPIO0-GPIO9
        adc1_channel_t channel = static_cast<adc1_channel_t>(m_config.battery_adc_pin);
        adc1_config_channel_atten(channel, ADC_ATTEN_DB_11);  // 0-3.3V range
        ESP_LOGI(TAG, "ADC initialized for battery monitoring (GPIO %d)", m_config.battery_adc_pin);
    } else {
        ESP_LOGW(TAG, "Battery ADC pin %d not configured (valid: 0-9)", m_config.battery_adc_pin);
    }
}

uint16_t PowerManager::readBatteryADC() {
    if (m_config.battery_adc_pin >= 10) {
        return 0;  // Invalid pin
    }
    
    adc1_channel_t channel = static_cast<adc1_channel_t>(m_config.battery_adc_pin);
    uint32_t sum = 0;
    const int samples = 10;
    
    for (int i = 0; i < samples; i++) {
        sum += adc1_get_raw(channel);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    
    return static_cast<uint16_t>(sum / samples);
}

float PowerManager::getBatteryVoltage() {
    uint16_t adc_value = readBatteryADC();
    
    if (adc_value == 0) {
        return 0.0f;  // Invalid reading
    }
    
    // ESP32-C3 ADC: 0-3.3V @ 12-bit (0-4095)
    // Assuming voltage divider: Vbat -> R1=100k -> ADC_PIN -> R2=100k -> GND
    // ADC reads Vbat/2, so multiply by 2
    float voltage = (adc_value / 4095.0f) * 3.3f * 2.0f;
    
    return voltage;
}

uint8_t PowerManager::getBatteryPercent() {
    float voltage = getBatteryVoltage();
    
    if (voltage == 0.0f) {
        return 0;  // Invalid reading
    }
    
    // Li-Ion battery voltage range: 3.0V (empty) to 4.2V (full)
    const float V_MIN = 3.0f;
    const float V_MAX = 4.2f;
    
    if (voltage >= V_MAX) return 100;
    if (voltage <= V_MIN) return 0;
    
    uint8_t percent = static_cast<uint8_t>(((voltage - V_MIN) / (V_MAX - V_MIN)) * 100.0f);
    return percent;
}

float PowerManager::measureCurrentConsumption() {
    // Note: ESP32-C3 doesn't have built-in current measurement
    // This is a placeholder that calculates based on power states
    // For actual measurement, use external current sensor (e.g., INA219)
    
    // Estimated consumption based on mode
    if (m_sensor_powered) {
        // Active mode: ESP32-C3 (~50mA) + Sensor (~1mA) = ~51mA
        return 51000.0f;  // µA
    } else {
        // Sleep mode: Deep sleep ~10µA
        return 10.0f;  // µA
    }
}

void PowerManager::updatePowerStats(uint32_t active_time_ms, uint32_t sleep_time_ms) {
    s_total_active_time_ms += active_time_ms;
    s_total_sleep_time_ms += sleep_time_ms;
    
    // Calculate average current consumption
    float active_current_ua = measureCurrentConsumption();
    float sleep_current_ua = 10.0f;  // Deep sleep current
    
    uint32_t total_time_ms = active_time_ms + sleep_time_ms;
    if (total_time_ms > 0) {
        m_stats.avg_current_ua = ((active_current_ua * active_time_ms) + 
                                  (sleep_current_ua * sleep_time_ms)) / total_time_ms;
    }
    
    m_stats.active_current_ma = active_current_ua / 1000.0f;
    m_stats.sleep_current_ua = sleep_current_ua;
    m_stats.total_active_time_ms = s_total_active_time_ms;
    m_stats.total_sleep_time_ms = s_total_sleep_time_ms;
    m_stats.wakeup_count = s_total_wakeups;
    
    // Calculate estimated battery life
    float battery_voltage = getBatteryVoltage();
    if (battery_voltage > 0.0f && m_stats.avg_current_ua > 0.0f) {
        // Battery capacity estimate (assuming 2000mAh)
        float battery_capacity_mah = 2000.0f;
        float daily_consumption_mah = (m_stats.avg_current_ua / 1000.0f) * 24.0f;
        m_stats.estimated_battery_life_days = battery_capacity_mah / daily_consumption_mah;
    }
}

float PowerManager::calculateBatteryLife(uint32_t battery_capacity_mah) const {
    if (m_stats.avg_current_ua <= 0.0f) {
        return 0.0f;
    }
    
    // Calculate daily consumption
    float daily_consumption_mah = (m_stats.avg_current_ua / 1000.0f) * 24.0f;
    
    if (daily_consumption_mah <= 0.0f) {
        return 0.0f;
    }
    
    // Return battery life in days
    return battery_capacity_mah / daily_consumption_mah;
}

void PowerManager::enableAutoSleep(bool enable) {
    m_config.enable_auto_sleep = enable;
    ESP_LOGI(TAG, "Auto-sleep %s", enable ? "enabled" : "disabled");
}

void PowerManager::saveStateToRTC() {
    // State is already saved in RTC_DATA_ATTR variables
    // This function can be used to save additional state if needed
    ESP_LOGD(TAG, "State saved to RTC memory");
}

void PowerManager::restoreStateFromRTC() {
    // Restore statistics from RTC memory
    m_stats.wakeup_count = s_total_wakeups;
    m_stats.total_active_time_ms = s_total_active_time_ms;
    m_stats.total_sleep_time_ms = s_total_sleep_time_ms;
    
    ESP_LOGI(TAG, "State restored from RTC:");
    ESP_LOGI(TAG, "  Total wake-ups: %u", (unsigned int)s_total_wakeups);
    ESP_LOGI(TAG, "  Total active time: %u ms", (unsigned int)s_total_active_time_ms);
    ESP_LOGI(TAG, "  Total sleep time: %u ms", (unsigned int)s_total_sleep_time_ms);
}

