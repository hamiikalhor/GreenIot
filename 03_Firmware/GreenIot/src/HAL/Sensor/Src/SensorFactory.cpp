/**
 * @file SensorFactory.cpp
 * @brief Sensor factory implementation
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-03
 */

#include "ISensor.hpp"
#include "SHT31Sensor.hpp"
// #include "AHT20Sensor.hpp"  // Add when implemented

#include <map>
#include <functional>
#include <memory>

std::unique_ptr<ISensor> SensorFactory::create(const std::string& name) {
    // Simple factory implementation (compatible with C++11)
    if (name == "SHT31") {
        return std::unique_ptr<ISensor>(new SHT31Sensor());
    }
    // Add more sensors here when implemented
    // if (name == "AHT20") {
    //     return std::unique_ptr<ISensor>(new AHT20Sensor());
    // }
    
    return nullptr;
}

std::vector<std::string> SensorFactory::getAvailableSensors() {
    return {
        "SHT31",
        // "AHT20",  // Add when implemented
    };
}

