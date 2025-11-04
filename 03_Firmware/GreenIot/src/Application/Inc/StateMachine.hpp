/**
 * @file StateMachine.hpp
 * @brief System state machine (main application logic)
 * 
 * Architecture Layer: APPLICATION LAYER
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include "ISensor.hpp"
#include <cstdint>
#include <memory>

enum class SystemState {
    INIT,
    IDLE,
    MEASURE,
    TRANSMIT,
    SLEEP,
    ERROR
};

struct SystemConfig {
    uint32_t measurement_interval_sec;
    uint32_t transmission_interval_sec;
    uint8_t max_retries;
    const char* sensor_type;
    
    SystemConfig()
        : measurement_interval_sec(60)    // 1 minute
        , transmission_interval_sec(300)  // 5 minutes
        , max_retries(3)
        , sensor_type("SHT31") {}
};

/**
 * @brief State Machine Class (Application Core)
 */
class StateMachine {
public:
    StateMachine();
    ~StateMachine() = default;
    
    void init(const SystemConfig& config);
    void run();  // Called in loop
    
    SystemState getState() const { return m_current_state; }
    
private:
    SystemState m_current_state;
    SystemState m_previous_state;
    SystemConfig m_config;
    
    std::unique_ptr<ISensor> m_sensor;
    SensorData m_last_reading;
    
    uint32_t m_last_measurement_time;
    uint32_t m_last_transmission_time;
    uint8_t m_retry_count;
    
    // State handlers
    void handleInit();
    void handleIdle();
    void handleMeasure();
    void handleTransmit();
    void handleSleep();
    void handleError();
    
    void transitionTo(SystemState new_state);
    uint32_t getUptime() const;
};

#endif // STATE_MACHINE_HPP

