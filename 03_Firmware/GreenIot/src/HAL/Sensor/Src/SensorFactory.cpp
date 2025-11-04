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

std::unique_ptr<ISensor> SensorFactory::create(const std::string& name) {
    // Sensor creation map
    static const std::map<std::string, std::function<std::unique_ptr<ISensor>()>> creators = {
        {"SHT31", []() { return std::make_unique<SHT31Sensor>(); }},
        // {"AHT20", []() { return std::make_unique<AHT20Sensor>(); }},  // Add when implemented
    };
    
    auto it = creators.find(name);
    if (it != creators.end()) {
        return it->second();
    }
    
    return nullptr;
}

std::vector<std::string> SensorFactory::getAvailableSensors() {
    return {
        "SHT31",
        // "AHT20",  // Add when implemented
    };
}

