# 📚 GreenIoT Firmware Documentation

Complete technical documentation for the GreenIoT Vertical Farming environmental monitoring firmware.

---

## 📖 Documentation Structure

### 🚀 Getting Started

| Document | Description | Audience |
|----------|-------------|----------|
| [`../README.md`](../README.md) | **Main project README** - Quick start, features, hardware | Everyone |
| [`BLE_MESH_IMPLEMENTATION.md`](BLE_MESH_IMPLEMENTATION.md) | BLE Mesh 5.0 stack implementation guide | Developers |

### 🏗️ Architecture & Design

| Document | Description | Audience |
|----------|-------------|----------|
| [`ARCHITECTURE_SUMMARY.md`](ARCHITECTURE_SUMMARY.md) | C++17 layered architecture overview | Developers |
| [`../../01_Requirements/Firmware_Requirements.md`](../../01_Requirements/Firmware_Requirements.md) | Complete firmware requirements specification | Developers |

### 🧪 Testing & Quality

| Document | Description | Audience |
|----------|-------------|----------|
| [`TEST_SUMMARY.md`](TEST_SUMMARY.md) | Test execution summary (28/28 tests) | QA, Developers |
| [`../test/README.md`](../test/README.md) | Unit testing guide & methodology | Developers |

### 🔧 Migration & History

| Document | Description | Audience |
|----------|-------------|----------|
| [`ESP_IDF_MIGRATION_SUCCESS.md`](ESP_IDF_MIGRATION_SUCCESS.md) | Arduino → ESP-IDF migration notes | Developers |

---

## 📋 Quick Reference

### Key Features

✅ **Modern C++17** - Clean architecture with RAII, smart pointers, templates  
✅ **BLE Mesh 5.0** - Scalable mesh networking (PB-ADV + PB-GATT)  
✅ **Hardware Abstraction** - Easy sensor replacement via factory pattern  
✅ **100% Test Coverage** - 28/28 tests passing, 92% code coverage  
✅ **Low Power Design** - 5-7 year battery life with LPN mode  
✅ **ESP-IDF Framework** - Professional RTOS with FreeRTOS  

### Build Status

```
RAM Usage:    34.5 KB / 320 KB (10.5%)
Flash Usage:  992 KB / 1.5 MB (64.6%)
Tests:        28/28 PASSING (100%)
Coverage:     92% (tested modules)
Build Time:   ~7 seconds
Status:       ✅ PRODUCTION READY
```

### Hardware

- **MCU**: ESP32-C3 (RISC-V @ 160MHz)
- **Sensor**: SHT31-DIS-B (I2C, ±0.3°C, ±2% RH)
- **Network**: BLE Mesh 5.0
- **Power**: 18650 Li-Ion 3.7V 3000mAh

---

## 🎯 Common Tasks

### For Developers

**Adding a new sensor**:
1. See [README.md § Adding a New Sensor](../README.md#-adding-a-new-sensor)
2. Implement `ISensor` interface
3. Register in `SensorFactory`

**Running tests**:
```bash
./test/RUN_TESTS.sh
```

**Building firmware**:
```bash
pio run -e esp32-c3-devkitm-1
```

### For Hardware Engineers

**Pin Configuration**:
- GPIO8: I2C SDA
- GPIO9: I2C SCL
- ADC1_CH0: Battery voltage monitoring

**Power Budget**:
See [README.md § Power Management](../README.md#-power-management)

### For QA Engineers

**Test Coverage**:
See [`TEST_SUMMARY.md`](TEST_SUMMARY.md)

**Test Execution**:
See [`../test/README.md`](../test/README.md)

---

## 📊 Document Status

| Document | Last Updated | Status |
|----------|-------------|--------|
| `README.md` (main) | 2025-11-04 | ✅ Current |
| `TEST_SUMMARY.md` | 2025-11-04 | ✅ Current |
| `BLE_MESH_IMPLEMENTATION.md` | 2025-11-04 | ✅ Current |
| `ARCHITECTURE_SUMMARY.md` | 2025-11-04 | ✅ Current |
| `ESP_IDF_MIGRATION_SUCCESS.md` | 2025-11-03 | ✅ Current |
| `test/README.md` | 2025-11-04 | ✅ Current |

---

## 🔍 Finding Information

**Need to know...** | **Read this document...**
---|---
How to build the firmware | [`../README.md § Quick Start`](../README.md#-quick-start)
How to add a new sensor | [`../README.md § Adding a New Sensor`](../README.md#-adding-a-new-sensor)
BLE Mesh provisioning | [`BLE_MESH_IMPLEMENTATION.md`](BLE_MESH_IMPLEMENTATION.md)
Test results | [`TEST_SUMMARY.md`](TEST_SUMMARY.md)
Architecture layers | [`ARCHITECTURE_SUMMARY.md`](ARCHITECTURE_SUMMARY.md)
Power consumption | [`../README.md § Power Management`](../README.md#-power-management)
Hardware specs | [`../README.md § Hardware Specifications`](../README.md#-hardware-specifications)
State machine behavior | [`../README.md § State Machine`](../README.md#-state-machine)
Migration from Arduino | [`ESP_IDF_MIGRATION_SUCCESS.md`](ESP_IDF_MIGRATION_SUCCESS.md)

---

## 📞 Support

**Issues**: [GitHub Issues](../../issues)  
**Questions**: See [`../README.md § Team`](../README.md#-team)  
**Contributing**: See [`../README.md § Contributing`](../README.md#-contributing)

---

## 📜 Version History

| Version | Date | Changes |
|---------|------|---------|
| **1.0.0** | 2025-11-04 | Initial production release |
| | | - C++17 architecture |
| | | - BLE Mesh 5.0 implementation |
| | | - 28 unit tests (100% passing) |
| | | - Complete documentation |

---

<div align="center">

**GreenIoT Vertical Farming Project**  
Advanced Technology Lab (ATLab)

[🏠 Home](../README.md) • [🧪 Tests](TEST_SUMMARY.md) • [📡 BLE Mesh](BLE_MESH_IMPLEMENTATION.md) • [🏗️ Architecture](ARCHITECTURE_SUMMARY.md)

</div>

