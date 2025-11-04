/**
 * @file PowerManager.cpp
 * @brief Power management implementation for ESP32-C3
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#include "PowerManager.hpp"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "POWER";

PowerManager& PowerManager::getInstance() {
    static PowerManager instance;
    return instance;
}

void PowerManager::init(const PowerConfig& config) {
    m_config = config;
    
    // Configure wakeup sources
    esp_sleep_enable_timer_wakeup(config.deep_sleep_duration_sec * 1000000ULL);
    
    // Initialize ADC for battery monitoring
    initADC();
    
    ESP_LOGI(TAG, "PowerManager initialized");
    ESP_LOGI(TAG, "  Deep sleep: %d sec", (int)config.deep_sleep_duration_sec);
    ESP_LOGI(TAG, "  Light sleep: %d ms", (int)config.light_sleep_duration_ms);
    
    m_initialized = true;
}

void PowerManager::enterLightSleep(uint32_t duration_ms) {
    ESP_LOGI(TAG, "Entering light sleep for %d ms", (int)duration_ms);
    
    esp_sleep_enable_timer_wakeup(duration_ms * 1000ULL);
    esp_light_sleep_start();
    
    ESP_LOGI(TAG, "Woke from light sleep");
}

void PowerManager::enterDeepSleep(uint32_t duration_sec) {
    ESP_LOGI(TAG, "Entering deep sleep for %d seconds", (int)duration_sec);
    
    esp_sleep_enable_timer_wakeup(duration_sec * 1000000ULL);
    esp_deep_sleep_start();
    
    // NOTE: Execution NEVER reaches here (device resets on wakeup)
}

WakeupSource PowerManager::getWakeupCause() {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    
    switch (cause) {
        case ESP_SLEEP_WAKEUP_TIMER:
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
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
}

uint16_t PowerManager::readBatteryADC() {
    uint32_t sum = 0;
    const int samples = 10;
    
    for (int i = 0; i < samples; i++) {
        sum += adc1_get_raw(ADC1_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    
    return static_cast<uint16_t>(sum / samples);
}

float PowerManager::getBatteryVoltage() {
    uint16_t adc_value = readBatteryADC();
    
    // ESP32-C3 ADC: 0-3.3V @ 12-bit (0-4095)
    // Assuming voltage divider: Vbat -> R1=100k -> ADC_PIN -> R2=100k -> GND
    // ADC reads Vbat/2, so multiply by 2
    float voltage = (adc_value / 4095.0f) * 3.3f * 2.0f;
    
    return voltage;
}

uint8_t PowerManager::getBatteryPercent() {
    float voltage = getBatteryVoltage();
    
    // Li-Ion battery voltage range: 3.0V (empty) to 4.2V (full)
    const float V_MIN = 3.0f;
    const float V_MAX = 4.2f;
    
    if (voltage >= V_MAX) return 100;
    if (voltage <= V_MIN) return 0;
    
    uint8_t percent = static_cast<uint8_t>(((voltage - V_MIN) / (V_MAX - V_MIN)) * 100.0f);
    return percent;
}

void PowerManager::enableAutoSleep(bool enable) {
    m_config.enable_auto_sleep = enable;
    ESP_LOGI(TAG, "Auto-sleep %s", enable ? "enabled" : "disabled");
}

