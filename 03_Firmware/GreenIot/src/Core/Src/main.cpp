/**
 * @file main.cpp
 * @brief Main application entry point (ESP-IDF)
 * 
 * Architecture Layer: CORE (Entry Point)
 * 
 * GreenIoT Vertical Farming - Basil Environmental Monitoring Node
 * 
 * Hardware: ESP32-C3
 * Sensor: SHT31 (Temperature & Humidity)
 * Communication: BLE Mesh (future phase)
 * Power: Battery-operated with sleep modes
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#include "StateMachine.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

static const char* TAG = "MAIN";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  GreenIoT Vertical Farming Node");
    ESP_LOGI(TAG, "  Basil Environmental Monitoring");
    ESP_LOGI(TAG, "  Hardware: ESP32-C3");
    ESP_LOGI(TAG, "  Firmware Version: 1.0.0");
    ESP_LOGI(TAG, "========================================");
    
    // Initialize NVS (Non-Volatile Storage) for ESP-IDF
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Create state machine
    static StateMachine state_machine;
    
    // Configure system
    SystemConfig config;
    config.measurement_interval_sec = 60;      // Measure every 1 minute
    config.transmission_interval_sec = 300;    // Transmit every 5 minutes
    config.max_retries = 3;
    config.sensor_type = "SHT31";
    
    // Initialize state machine
    state_machine.init(config);
    
    ESP_LOGI(TAG, "Entering main loop...");
    
    // Main loop
    while (true) {
        state_machine.run();
        
        // Small yield to watchdog/scheduler
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

