/**
 * @file PowerManager.hpp
 * @brief Power management service (battery & sleep control)
 * 
 * Architecture Layer: SERVICE LAYER
 * 
 * Features:
 * - Deep sleep with periodic wake-up timer
 * - GPIO control for sensor power pin
 * - Current consumption measurement
 * - Battery voltage monitoring
 * - RTC memory for state preservation
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
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
    UNKNOWN,
    POWER_ON  // First boot
};

struct PowerConfig {
    uint32_t deep_sleep_duration_sec;
    uint32_t light_sleep_duration_ms;
    bool enable_auto_sleep;
    uint16_t battery_adc_pin;
    uint8_t sensor_power_pin;      // GPIO pin for sensor power control
    bool enable_sensor_power_control;  // Enable GPIO power control
    
    PowerConfig()
        : deep_sleep_duration_sec(300)   // 5 minutes
        , light_sleep_duration_ms(100)
        , enable_auto_sleep(false)
        , battery_adc_pin(0)
        , sensor_power_pin(10)           // GPIO 10 for sensor power
        , enable_sensor_power_control(true) {}
};

/**
 * @brief Power consumption statistics
 */
struct PowerStats {
    float avg_current_ua;        // Average current consumption (µA)
    float active_current_ma;      // Active mode current (mA)
    float sleep_current_ua;       // Sleep mode current (µA)
    uint32_t total_active_time_ms;  // Total active time
    uint32_t total_sleep_time_ms;   // Total sleep time
    uint32_t wakeup_count;         // Number of wake-ups
    float estimated_battery_life_days;  // Estimated battery life
    
    PowerStats()
        : avg_current_ua(0.0f)
        , active_current_ma(0.0f)
        , sleep_current_ua(0.0f)
        , total_active_time_ms(0)
        , total_sleep_time_ms(0)
        , wakeup_count(0)
        , estimated_battery_life_days(0.0f) {}
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
    
    // Sensor power control
    void sensorPowerOn();
    void sensorPowerOff();
    bool isSensorPowered() const { return m_sensor_powered; }
    
    // Periodic wake-up timer
    void configureWakeupTimer(uint32_t duration_sec);
    uint32_t getWakeupTimerDuration() const { return m_config.deep_sleep_duration_sec; }
    
    // Battery monitoring
    float getBatteryVoltage();
    uint8_t getBatteryPercent();
    
    // Current consumption measurement
    float measureCurrentConsumption();  // Returns current in µA
    PowerStats getPowerStats() const { return m_stats; }
    void updatePowerStats(uint32_t active_time_ms, uint32_t sleep_time_ms);
    float calculateBatteryLife(uint32_t battery_capacity_mah) const;
    
    // Auto-sleep
    void enableAutoSleep(bool enable);
    bool isAutoSleepEnabled() const { return m_config.enable_auto_sleep; }
    
    // State preservation (RTC memory)
    void saveStateToRTC();
    void restoreStateFromRTC();
    
private:
    PowerManager() 
        : m_initialized(false)
        , m_sensor_powered(false) {}
    ~PowerManager() = default;
    
    bool m_initialized;
    bool m_sensor_powered;
    PowerConfig m_config;
    PowerStats m_stats;
    
    void initADC();
    void initGPIO();
    uint16_t readBatteryADC();
    void updateCurrentConsumption();
};

#endif // POWER_MANAGER_HPP

