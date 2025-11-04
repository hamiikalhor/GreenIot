/**
 * @file PowerManager.hpp
 * @brief Power management service (battery & sleep control)
 * 
 * Architecture Layer: SERVICE LAYER
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#ifndef POWER_MANAGER_HPP
#define POWER_MANAGER_HPP

#include <cstdint>

enum class SleepMode {
    LIGHT_SLEEP,
    DEEP_SLEEP
};

enum class WakeupSource {
    TIMER,
    BUTTON,
    UNKNOWN
};

struct PowerConfig {
    uint32_t deep_sleep_duration_sec;
    uint32_t light_sleep_duration_ms;
    bool enable_auto_sleep;
    uint16_t battery_adc_pin;
    
    PowerConfig()
        : deep_sleep_duration_sec(300)   // 5 minutes
        , light_sleep_duration_ms(100)
        , enable_auto_sleep(false)
        , battery_adc_pin(0) {}
};

/**
 * @brief Power Manager Service (Singleton)
 */
class PowerManager {
public:
    static PowerManager& getInstance();
    
    // Delete copy
    PowerManager(const PowerManager&) = delete;
    PowerManager& operator=(const PowerManager&) = delete;
    
    void init(const PowerConfig& config);
    void enterLightSleep(uint32_t duration_ms);
    void enterDeepSleep(uint32_t duration_sec);
    WakeupSource getWakeupCause();
    
    // Battery monitoring
    float getBatteryVoltage();
    uint8_t getBatteryPercent();
    
    void enableAutoSleep(bool enable);
    
private:
    PowerManager() : m_initialized(false) {}
    ~PowerManager() = default;
    
    bool m_initialized;
    PowerConfig m_config;
    
    void initADC();
    uint16_t readBatteryADC();
};

#endif // POWER_MANAGER_HPP

