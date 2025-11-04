# Test Suite

This directory contains all test files for the GreenIoT BLE Mesh Sensor Node.

---

## 📁 Test Files

### Unit Tests

- **`test_ble_mesh.cpp`** - Hardware tests (requires ESP32-C3)
  - 15 BLE Mesh interface tests
  - Configuration validation
  - Provisioning tests
  - Publishing tests

- **`test_ble_mesh_with_mocks.cpp`** - Native tests (PC-based)
  - 15 mock tests
  - No hardware required
  - Fast execution (~2 seconds)
  - ✅ **15/15 PASSING**

### Mock Files

- **`mocks/mock_ble_mesh.cpp`** - BLE Mesh mock implementation
- **`mocks/mock_ble_mesh.h`** - Mock helper functions
- **`mocks/Arduino_stub.h`** - Arduino compatibility stubs

---

## 🚀 Running Tests

### Quick Start

```bash
# From project root
./run_tests.sh mock    # Run mock tests (no hardware)
./run_tests.sh hw      # Run hardware tests (requires ESP32-C3)
./run_tests.sh all     # Run all tests
```

### Using PlatformIO

```bash
# Native mock tests
platformio test -e native -f test_ble_mesh_with_mocks

# ESP32-C3 hardware tests
platformio test -e esp32-c3-devkitm-1 -f test_ble_mesh
```

---

## ✅ Test Results

### Latest Run (Mock Tests)

```
Processing * in native environment
Building...
Testing...
================= 15 test cases: 15 succeeded in 00:00:00.834 =================
```

**Status:** ✅ All tests passing

---

## 📚 Documentation

For detailed testing procedures, see:
- **`docs/TESTING_GUIDE.md`** - Complete testing guide
- **`HARDWARE_TEST_CHECKLIST.md`** - Hardware test checklist
- **Main `README.md`** - Project overview

---

**Last Updated:** November 4, 2025
