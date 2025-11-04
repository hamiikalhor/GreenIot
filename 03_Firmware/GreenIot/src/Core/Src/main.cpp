/**
 * @file main.cpp
 * @brief Main application entry point (Arduino Framework)
 * 
 * Architecture Layer: CORE (Entry Point)
 * 
 * GreenIoT Vertical Farming - Basil Environmental Monitoring Node
 * 
 * Hardware: ESP32-C3
 * Sensor: SHT31 (Temperature & Humidity)
 * Communication: BLE Mesh
 * Power: Battery-operated with deep sleep modes
 * 
 * Deep Sleep Implementation (Issue #4):
 * - Device wakes periodically for sensor measurements
 * - Uses RTC timer for wake-up
 * - State preserved in RTC memory
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 */

#include <Arduino.h>
#include "StateMachine.hpp"
#include "PowerManager.hpp"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

static const char* TAG = "MAIN";

// Global state machine instance
static StateMachine* g_state_machine = nullptr;

void setup() {
    Serial.begin(115200);
    delay(1000);  // Allow serial monitor to connect
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  GreenIoT Vertical Farming Node");
    ESP_LOGI(TAG, "  Basil Environmental Monitoring");
    ESP_LOGI(TAG, "  Hardware: ESP32-C3");
    ESP_LOGI(TAG, "  Firmware Version: 1.0.0");
    ESP_LOGI(TAG, "  Issue #4: Deep Sleep & Wake-up");
    ESP_LOGI(TAG, "========================================");
    
    // Check wake-up cause (before PowerManager init)
    esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
    if (wakeup_cause == ESP_SLEEP_WAKEUP_UNDEFINED) {
        ESP_LOGI(TAG, "First boot or power-on reset");
    } else {
        ESP_LOGI(TAG, "Wake-up from deep sleep");
        ESP_LOGI(TAG, "  Cause: %s", 
                 wakeup_cause == ESP_SLEEP_WAKEUP_TIMER ? "Timer" :
                 wakeup_cause == ESP_SLEEP_WAKEUP_EXT0 ? "External GPIO (EXT0)" :
                 wakeup_cause == ESP_SLEEP_WAKEUP_EXT1 ? "External GPIO (EXT1)" :
                 "Unknown");
    }
    
    // Initialize NVS (Non-Volatile Storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Create state machine
    g_state_machine = new StateMachine();
    
    // Configure system
    SystemConfig config;
    config.measurement_interval_sec = 300;    // Measure every 5 minutes (deep sleep interval)
    config.transmission_interval_sec = 300;  // Transmit every 5 minutes
    config.max_retries = 3;
    config.sensor_type = "SHT31";
    
    // Initialize state machine
    g_state_machine->init(config);
    
    ESP_LOGI(TAG, "Entering main loop...");
    ESP_LOGI(TAG, "  Measurement interval: %d sec", (int)config.measurement_interval_sec);
    ESP_LOGI(TAG, "  Transmission interval: %d sec", (int)config.transmission_interval_sec);
}

void loop() {
    if (g_state_machine) {
        g_state_machine->run();
        
        // Small yield to watchdog/scheduler
        delay(10);
    } else {
        ESP_LOGE(TAG, "State machine not initialized!");
        delay(1000);
    }
    
    // Note: When deep sleep is triggered in StateMachine::handleSleep(),
    // device will reset and restart from setup()
}

