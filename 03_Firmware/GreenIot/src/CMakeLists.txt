# Main component CMakeLists.txt for ESP-IDF
# This file must be in main/ folder as per ESP-IDF convention
# All layered architecture folders are now inside main/

# All source files (relative to main/ folder)
file(GLOB_RECURSE SRCS
    "${CMAKE_CURRENT_LIST_DIR}/Core/Src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/Application/Src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Application/Src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/Services/Src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Services/Src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/HAL/Sensor/Src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/HAL/Sensor/Src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/HAL/Wireless/Src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/HAL/Wireless/Src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/Drivers/Src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Drivers/Src/*.c"
)

# All include directories
set(INCLUDES
    "${CMAKE_CURRENT_LIST_DIR}/Core/Inc"
    "${CMAKE_CURRENT_LIST_DIR}/Application/Inc"
    "${CMAKE_CURRENT_LIST_DIR}/Services/Inc"
    "${CMAKE_CURRENT_LIST_DIR}/HAL/Sensor/Inc"
    "${CMAKE_CURRENT_LIST_DIR}/HAL/Wireless/Inc"
    "${CMAKE_CURRENT_LIST_DIR}/Drivers/Inc"
)

# Register component
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS ${INCLUDES}
    REQUIRES
        driver
        esp_timer
        nvs_flash
        esp_pm
        esp_adc
)

# Set C++ compilation flags
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -std=gnu++17
)

