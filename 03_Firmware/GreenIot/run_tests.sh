#!/bin/bash

################################################################################
# GreenIoT BLE Mesh Sensor Node - Test Runner Script
#
# This script provides easy access to all test suites
#
# Usage:
#   ./run_tests.sh [option]
#
# Options:
#   mock     - Run BLE Mesh mock tests (PC-based, no hardware)
#   hw       - Run hardware tests (requires ESP32-C3)
#   all      - Run all tests
#   clean    - Clean test builds
#   help     - Show this help message
#
# Author: GreenIoT Vertical Farming Project
# Date: November 4, 2025
################################################################################

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Find platformio
if [ -f "$HOME/.platformio/penv/bin/platformio" ]; then
    PIO="$HOME/.platformio/penv/bin/platformio"
elif command -v platformio &> /dev/null; then
    PIO="platformio"
elif command -v pio &> /dev/null; then
    PIO="pio"
else
    echo -e "${RED}Error: PlatformIO not found!${NC}"
    echo "Please install PlatformIO: https://platformio.org/"
    exit 1
fi

################################################################################
# Functions
################################################################################

print_header() {
    echo -e "${BLUE}"
    echo "╔════════════════════════════════════════════════════════════╗"
    echo "║                                                            ║"
    echo "║     GreenIoT BLE Mesh Sensor Node - Test Suite            ║"
    echo "║                                                            ║"
    echo "╚════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

run_mock_tests() {
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${YELLOW}Running BLE Mesh Mock Tests (Native/PC-based)${NC}"
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    
    # Temporarily hide other test files to avoid linker errors
    test -f test/test_ble_mesh.cpp && mv test/test_ble_mesh.cpp test/test_ble_mesh.cpp.tmp
    test -f test/test_sensor_native.cpp && mv test/test_sensor_native.cpp test/test_sensor_native.cpp.tmp
    
    $PIO test -e native -f test_ble_mesh_with_mocks
    local result=$?
    
    # Restore test files
    test -f test/test_ble_mesh.cpp.tmp && mv test/test_ble_mesh.cpp.tmp test/test_ble_mesh.cpp
    test -f test/test_sensor_native.cpp.tmp && mv test/test_sensor_native.cpp.tmp test/test_sensor_native.cpp
    
    if [ $result -eq 0 ]; then
        echo ""
        echo -e "${GREEN}✅ Mock tests PASSED!${NC}"
        return 0
    else
        echo ""
        echo -e "${RED}❌ Mock tests FAILED!${NC}"
        return 1
    fi
}

run_hardware_tests() {
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${YELLOW}Running Hardware Tests (ESP32-C3 required)${NC}"
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "${BLUE}Note: This requires ESP32-C3 hardware connected via USB${NC}"
    echo ""
    
    # Check if device is connected
    if ! $PIO device list | grep -q "tty"; then
        echo -e "${RED}❌ No ESP32 device detected!${NC}"
        echo "Please connect ESP32-C3 via USB and try again."
        return 1
    fi
    
    $PIO test -e esp32-c3-devkitm-1 -f test_ble_mesh
    
    if [ $? -eq 0 ]; then
        echo ""
        echo -e "${GREEN}✅ Hardware tests PASSED!${NC}"
        return 0
    else
        echo ""
        echo -e "${RED}❌ Hardware tests FAILED!${NC}"
        return 1
    fi
}

run_all_tests() {
    local failed=0
    
    echo ""
    run_mock_tests || failed=1
    
    echo ""
    echo ""
    run_hardware_tests || failed=1
    
    echo ""
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    if [ $failed -eq 0 ]; then
        echo -e "${GREEN}✅ ALL TESTS PASSED!${NC}"
    else
        echo -e "${RED}❌ SOME TESTS FAILED!${NC}"
    fi
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    
    return $failed
}

clean_tests() {
    echo -e "${YELLOW}Cleaning test builds...${NC}"
    $PIO test -e native -c
    $PIO test -e esp32-c3-devkitm-1 -c
    echo -e "${GREEN}✅ Test builds cleaned!${NC}"
}

show_help() {
    echo "GreenIoT BLE Mesh Sensor Node - Test Runner"
    echo ""
    echo "Usage: ./run_tests.sh [option]"
    echo ""
    echo "Options:"
    echo "  mock     - Run BLE Mesh mock tests (PC-based, no hardware)"
    echo "  hw       - Run hardware tests (requires ESP32-C3)"
    echo "  all      - Run all tests"
    echo "  clean    - Clean test builds"
    echo "  help     - Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./run_tests.sh mock          # Quick mock tests"
    echo "  ./run_tests.sh hw            # Hardware validation"
    echo "  ./run_tests.sh all           # Complete test suite"
    echo ""
    echo "Test Files:"
    echo "  - test/test_ble_mesh_with_mocks.cpp  (Native mock tests)"
    echo "  - test/test_ble_mesh.cpp             (ESP32-C3 hardware tests)"
    echo ""
    echo "Documentation:"
    echo "  - TEST_RESULTS.md          (Test results and metrics)"
    echo "  - docs/TESTING_GUIDE.md    (Complete testing guide)"
    echo ""
}

show_test_summary() {
    echo ""
    echo -e "${BLUE}Test Summary:${NC}"
    echo "  - Mock Tests: 15 tests (configuration, init, provisioning, publishing)"
    echo "  - Duration: ~2 seconds"
    echo "  - Platform: Native (Mac/Linux/Windows)"
    echo "  - Hardware Required: None"
    echo ""
}

################################################################################
# Main
################################################################################

print_header

case "${1:-mock}" in
    mock|m)
        run_mock_tests
        show_test_summary
        ;;
    hw|hardware)
        run_hardware_tests
        ;;
    all|a)
        run_all_tests
        show_test_summary
        ;;
    clean|c)
        clean_tests
        ;;
    help|h|-h|--help)
        show_help
        ;;
    *)
        echo -e "${RED}Unknown option: $1${NC}"
        echo ""
        show_help
        exit 1
        ;;
esac

exit $?

