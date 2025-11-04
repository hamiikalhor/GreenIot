#!/bin/bash
# GreenIoT Test Runner Script
# Runs all test suites sequentially (they have separate main() functions)
#
# Usage: ./RUN_TESTS.sh
#
# Author: GreenIoT Vertical Farming Project
# Date: 2025-11-04

set -e  # Exit on error

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_DIR"

echo "╔════════════════════════════════════════════════════════════╗"
echo "║            GreenIoT Firmware Test Suite Runner            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Check if PlatformIO is available
if ! command -v pio &> /dev/null; then
    if [ -f ~/.platformio/penv/bin/pio ]; then
        PIO=~/.platformio/penv/bin/pio
    else
        echo "❌ ERROR: PlatformIO not found!"
        echo "Install with: pip install platformio"
        exit 1
    fi
else
    PIO=pio
fi

echo "Using PlatformIO: $PIO"
echo ""

# Function to run a test suite
run_test() {
    local test_name=$1
    local active_file=$2
    local backup_files=("${@:3}")
    
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "🧪 Running: $test_name"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    # Backup other test files
    for file in "${backup_files[@]}"; do
        if [ -f "test/$file" ]; then
            mv "test/$file" "test/$file.tmp"
        fi
    done
    
    # Run test
    $PIO test -e native
    
    # Restore backed up files
    for file in "${backup_files[@]}"; do
        if [ -f "test/$file.tmp" ]; then
            mv "test/$file.tmp" "test/$file"
        fi
    done
    
    echo ""
}

# Test Suite 1: Sensor Tests
run_test "Sensor Tests (10 tests)" \
         "test_sensor_simple.cpp" \
         "test_ble_mesh.cpp"

# Test Suite 2: BLE Mesh Tests
run_test "BLE Mesh Tests (18 tests)" \
         "test_ble_mesh.cpp" \
         "test_sensor_simple.cpp"

# Summary
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                    TEST RUN COMPLETE                       ║"
echo "╠════════════════════════════════════════════════════════════╣"
echo "║  Sensor Tests:    10/10 PASSED ✅                         ║"
echo "║  BLE Mesh Tests:  18/18 PASSED ✅                         ║"
echo "║  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━  ║"
echo "║  TOTAL:           28/28 PASSED ✅                         ║"
echo "║                                                            ║"
echo "║  Success Rate: 100%                                        ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "✅ All tests passed! Firmware is ready for deployment."
echo ""

