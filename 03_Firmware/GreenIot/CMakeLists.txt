# CMakeLists.txt for GreenIoT Firmware (ESP-IDF)
# Modern C++ architecture with layered design

cmake_minimum_required(VERSION 3.16)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project definition
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(GreenIot)
