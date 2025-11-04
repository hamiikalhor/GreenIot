# 🧪 Test Suite

**Unit and integration tests for GreenIoT BLE Mesh Sensor Node**

---

## 📁 Test Files

### Mock Tests (PC-Based)
- **`test_ble_mesh_with_mocks.cpp`** - 15 BLE Mesh interface tests
  - No hardware required
  - Runs on any PC (Mac/Linux/Windows)
  - Duration: ~2 seconds
  - Status: ✅ 15/15 passing

### Hardware Tests (ESP32-C3)
- **`test_ble_mesh.cpp`** - BLE Mesh hardware validation
  - Requires ESP32-C3-DevKitM-1
  - Tests actual BLE Mesh stack
  - Duration: ~30 seconds
  - Status: ⏳ Pending hardware

### Mocks & Stubs
- **`mocks/mock_ble_mesh.cpp`** - BLE Mesh mock implementation
- **`mocks/mock_ble_mesh.h`** - Mock helper functions
- **`mocks/Arduino_stub.h`** - Arduino compatibility stubs

---

## 🚀 Quick Start

```bash
# Run mock tests (no hardware)
./run_tests.sh mock

# Run hardware tests (requires ESP32-C3)
./run_tests.sh hw

# Run all tests
./run_tests.sh all
```

---

## 📊 Test Coverage

| Component | Tests | Status |
|-----------|-------|--------|
| BLE Mesh Interface | 15 | ✅ Passing |
| Configuration | 2 | ✅ Passing |
| Initialization | 3 | ✅ Passing |
| Provisioning | 2 | ✅ Passing |
| Publishing | 3 | ✅ Passing |
| Status & Reset | 3 | ✅ Passing |
| Integration | 2 | ✅ Passing |

**Total:** 15/15 tests passing

---

## 📚 Documentation

For complete testing procedures and guides, see:
- **Complete Testing Guide:** `../docs/TESTING.md`
- **Hardware Checklist:** `../docs/HARDWARE_CHECKLIST.md`
- **Main README:** `../README.md`

---

## 🛠️ Test Framework

- **Unit Testing:** Unity (ThrowTheSwitch)
- **Mock Framework:** Custom (embedded in test files)
- **Build System:** PlatformIO
- **Platforms:** Native (PC) + ESP32-C3

---

**For detailed procedures, see** [`../docs/TESTING.md`](../docs/TESTING.md)
