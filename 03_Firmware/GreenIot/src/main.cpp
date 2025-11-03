/**
 * @file main.cpp
 * @brief GreenIoT Vertical Farming Sensor Node - Entry Point
 * 
 * This is a clean entry point that delegates all system management to the
 * System class. The main.cpp file is kept minimal and focused on the
 * Arduino framework requirements (setup/loop).
 * 
 * Architecture Layers:
 * 1. Application Layer (System class) - business logic
 * 2. HAL Layer (sensor_interface) - sensor abstraction
 * 3. Peripheral Driver Layer (i2c_driver) - hardware abstraction
 * 4. Hardware Layer (ESP32-C3, Wire.h) - actual hardware
 * 
 * Hardware:
 * - ESP32-C3 DevKit M-1
 * - SHT31 Temperature & Humidity Sensor (I2C)
 * 
 * Connections:
 * - SDA: GPIO 8
 * - SCL: GPIO 9
 * - VCC: 3.3V
 * - GND: GND
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 * @version 2.1 (System Architecture)
 */

#include <Arduino.h>
#include "system.h"
#include "esp_log.h"

// ============================================================================
// Configuration
// ============================================================================

#define SERIAL_BAUD 115200

static const char *TAG = "Main";

// ============================================================================
// Global Objects
// ============================================================================

System greeniot;  // Note: 'system' conflicts with stdlib, so we use 'greeniot'

// ============================================================================
// Setup
// ============================================================================

void setup() {
    // Initialize serial communication (for debug output)
    Serial.begin(SERIAL_BAUD);
    delay(1000);  // Wait for serial to stabilize
    
    // Set global log level
    esp_log_level_set("*", ESP_LOG_INFO);
    
    // Initialize system
    if (!greeniot.init()) {
        ESP_LOGE(TAG, "System initialization failed!");
        ESP_LOGE(TAG, "Halting execution...");
        while (true) {
            delay(1000);
        }
    }
    
    ESP_LOGI(TAG, "System initialized successfully");
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
    // Run system (handles measurement timing internally)
    greeniot.run();
    
    // Small delay to prevent tight loop
    delay(100);
}