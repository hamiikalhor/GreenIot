# Testing Guide - GreenIoT BLE Mesh Sensor Node

**Project**: GreenIoT Vertical Farming  
**Platform**: ESP32-C3 + ESP-IDF  
**Version**: 2.0  
**Date**: November 4, 2025

---

## 📋 Table of Contents

- [Quick Start](#quick-start)
- [Unit Testing](#unit-testing)
- [Hardware Testing](#hardware-testing)
- [Multi-Node Testing](#multi-node-testing)
- [Power Consumption Testing](#power-consumption-testing)
- [Test Analysis](#test-analysis)

---

## 🚀 Quick Start

### Running Tests

```bash
# Navigate to project
cd /Users/hamidrezakalhor/Desktop/GreenIot/03_Firmware/GreenIot

# Run mock tests (no hardware required)
./run_tests.sh mock

# Run hardware tests (requires ESP32-C3)
./run_tests.sh hw

# Show help
./run_tests.sh help
```

### Expected Results

```
✅ 15/15 mock tests passing in ~2 seconds
✅ No hardware required
✅ CI/CD ready
```

---

## 🧪 Unit Testing

### Test Environment

**Native Tests (PC-based)**
- Platform: Mac/Linux/Windows
- Duration: ~2 seconds
- Hardware: None required
- Framework: Unity + Mocks

**Hardware Tests (ESP32-C3)**
- Platform: ESP32-C3-DevKitM-1
- Duration: ~30 seconds
- Hardware: ESP32-C3 required
- Framework: Unity

### Running Unit Tests

#### Option 1: Test Runner Script (Recommended)

```bash
# Mock tests
./run_tests.sh mock

# Hardware tests
./run_tests.sh hw

# All tests
./run_tests.sh all
```

#### Option 2: PlatformIO Directly

```bash
# Native mock tests
platformio test -e native -f test_ble_mesh_with_mocks

# ESP32-C3 hardware tests
platformio test -e esp32-c3-devkitm-1 -f test_ble_mesh
```

### Test Coverage

#### BLE Mesh Tests (15 tests)

| Test | Description | Status |
|------|-------------|--------|
| `test_ble_mesh_config_intervals` | Validates 5-minute interval | ✅ PASS |
| `test_ble_mesh_property_ids` | Validates property IDs | ✅ PASS |
| `test_ble_mesh_init_with_valid_config` | Init with valid config | ✅ PASS |
| `test_ble_mesh_init_with_null_config` | Reject null config | ✅ PASS |
| `test_ble_mesh_double_init` | Prevent double init | ✅ PASS |
| `test_ble_mesh_provisioning_enable` | Enable provisioning | ✅ PASS |
| `test_ble_mesh_is_provisioned_initially_false` | Check initial state | ✅ PASS |
| `test_ble_mesh_publish_sensor_data_valid` | Publish valid data | ✅ PASS |
| `test_ble_mesh_publish_sensor_data_null` | Reject null data | ✅ PASS |
| `test_ble_mesh_publish_without_init` | Fail gracefully | ✅ PASS |
| `test_ble_mesh_get_status_valid` | Get status | ✅ PASS |
| `test_ble_mesh_status_to_string` | Convert status | ✅ PASS |
| `test_ble_mesh_prov_state_to_string` | Convert prov state | ✅ PASS |
| `test_ble_mesh_factory_reset` | Factory reset | ✅ PASS |
| `test_ble_mesh_full_workflow` | Complete workflow | ✅ PASS |

### Test Results

**Latest Run:**
```
Processing * in native environment
Building...
Testing...
================= 15 test cases: 15 succeeded in 00:00:00.834 =================
```

**Metrics:**
- Total Tests: 15
- Passed: 15 (100%)
- Failed: 0 (0%)
- Duration: 0.834 seconds
- Platform: Native (x86_64/arm64)

---

## 🔌 Hardware Testing

### Hardware Requirements

**Minimum:**
- 1× ESP32-C3-DevKitM-1
- 1× SHT31 sensor
- 1× USB cable (data-capable)

**Recommended:**
- 3-5× ESP32-C3-DevKitM-1
- 2× SHT31 sensors
- 2× 2000mAh LiPo batteries
- USB cables
- Breadboards

### Setup Procedure

#### 1. Hardware Assembly

**SHT31 Connections:**
```
SHT31          ESP32-C3
-----          --------
VCC    ─────>  3.3V
GND    ─────>  GND
SDA    ─────>  GPIO 8 (I2C SDA)
SCL    ─────>  GPIO 9 (I2C SCL)
```

**Power:**
- USB: 5V via USB-C
- Battery: 3.7V LiPo to VIN pin

#### 2. Firmware Upload

```bash
# Build and upload
platformio run -e esp32-c3-devkitm-1 -t upload

# Monitor serial output
platformio device monitor -p /dev/cu.usbserial-XXXXX
```

#### 3. Serial Monitor Verification

**Expected Output:**
```
[SYSTEM] GreenIoT Sensor Node v1.0.0
[SYSTEM] ESP32-C3 @ 160MHz
[I2C] Initializing I2C bus...
[I2C] I2C initialized: SDA=8, SCL=9
[SENSOR] Scanning I2C bus...
[SENSOR] Found device at 0x44 (SHT31)
[BLE_MESH] Initializing BLE Mesh...
[BLE_MESH] BLE Mesh initialized
[BLE_MESH] Provisioning enabled
[SYSTEM] System ready
```

#### 4. Basic Functionality Tests

**Test 1: Sensor Reading**
```
Expected every 5 minutes:
[SENSOR] Temperature: 22.5°C
[SENSOR] Humidity: 65.0%
[SENSOR] Assessment: ✅ OPTIMAL for basil
```

**Test 2: BLE Mesh Publishing**
```
Expected every 5 minutes:
[BLE_MESH] Publishing sensor data to 0xC000
  Property 0x004F: 2250 (temp)
  Property 0x0076: 6500 (humidity)
  Battery: 95%
  TTL: 7
```

### Troubleshooting

**Issue: Sensor not detected**
```
Solution:
1. Check I2C connections (SDA/SCL)
2. Verify 3.3V power to sensor
3. Add 4.7kΩ pull-up resistors if needed
4. Try different I2C address (0x44 or 0x45)
```

**Issue: BLE Mesh not starting**
```
Solution:
1. Check partition table (4MB flash required)
2. Verify Bluetooth enabled in sdkconfig
3. Check memory availability
4. Review error logs
```

---

## 🌐 Multi-Node Testing

### Network Setup

**Minimum Setup (3 nodes):**
```
[Node 1 (LPN)] ←5m→ [Node 2 (Friend)] ←5m→ [Gateway]
  + SHT31              Relay only            Provisioner
```

**Recommended Setup (5 nodes):**
```
      [Node 1]────┐                    ┌────[Gateway]
       (LPN)      │                    │      (Prov)
      +SHT31      │                    │
                  │                    │
                  └─[Node 2]─[Node 3]──┘
                    (Friend) (Friend)
                  ┌─[Node 4]───────────┘
      [Node 5]────┘ (Friend)
       (LPN)
      +SHT31
```

### Provisioning

#### Option 1: nRF Mesh App (Recommended)

1. **Download nRF Mesh App**
   - iOS: App Store
   - Android: Google Play

2. **Create Network**
   - Open app → Create Network
   - Name: "GreenIoT-Basil-Farm"
   - NetKey: Auto-generated

3. **Provision Nodes**
   ```
   Node 1: 0x0001 (LPN + SHT31)
   Node 2: 0x0002 (Friend/Relay)
   Node 3: 0x0003 (Friend/Relay)
   Gateway: 0x0004 (Provisioner)
   Node 5: 0x0005 (LPN + SHT31)
   ```

4. **Configure Each Node**
   - Add AppKey
   - Subscribe to group: 0xC000
   - Set publish address: 0xC000
   - Set TTL: 7

#### Option 2: ESP Provisioner

```bash
# Flash provisioner firmware to gateway
cd $IDF_PATH/examples/bluetooth/esp_ble_mesh/ble_mesh_provisioner
idf.py build flash monitor

# Provision via serial commands
prov enable
prov scan on
prov add <device_uuid>
prov assign 0x0001
```

### Test Procedures

#### Test 1: Basic Connectivity

**Objective:** Verify all nodes are provisioned and reachable

**Procedure:**
```bash
# On Gateway
mesh ping 0x0001  # Node 1
mesh ping 0x0002  # Node 2
mesh ping 0x0003  # Node 3

# Expected: ACK from each node < 100ms
```

**Success Criteria:**
- ✅ All nodes respond
- ✅ Response time < 100ms
- ✅ No packet loss

---

#### Test 2: Direct Communication (0 hops)

**Setup:**
- Node 1 within 5m of Gateway
- Other nodes powered off

**Procedure:**
1. Monitor Gateway serial
2. Node 1 publishes sensor data
3. Verify receipt at Gateway

**Expected Output (Gateway):**
```
[BLE_MESH] Received sensor data from 0x0001
  Temperature: 22.5°C
  Humidity: 65.0%
  RSSI: -45 dBm
  Hops: 0 (direct)
  Delay: 50ms
```

**Success Criteria:**
- ✅ Data received < 1 second
- ✅ Hop count = 0
- ✅ RSSI > -70 dBm
- ✅ No data corruption

---

#### Test 3: Single-Hop Relay

**Setup:**
```
[Node 1] ←5m→ [Node 2] ←5m→ [Gateway]
  LPN        Friend       Prov
  (>10m from Gateway)
```

**Procedure:**
1. Position Node 1 out of Gateway range (>10m)
2. Place Node 2 midway
3. Node 1 publishes
4. Node 2 relays
5. Gateway receives

**Expected Output (Node 2 - Relay):**
```
[BLE_MESH] Relaying message from 0x0001
  Src: 0x0001
  Dst: 0xC000 (group)
  TTL: 6 (was 7)
  OpCode: 0x52 (Sensor Status)
```

**Expected Output (Gateway):**
```
[BLE_MESH] Received sensor data from 0x0001 via 0x0002
  Hops: 1
  Relay: 0x0002
  Delay: 120ms
```

**Success Criteria:**
- ✅ Data received < 2 seconds
- ✅ Hop count = 1
- ✅ TTL decremented (7→6)
- ✅ Relay node logged

---

#### Test 4: Multi-Hop Relay

**Setup:**
```
[Node 1] ←5m→ [Node 2] ←5m→ [Node 3] ←5m→ [Gateway]
```

**Expected Path:**
```
Node 1 → Node 2 → Node 3 → Gateway
TTL:7     TTL:6     TTL:5     Recv
```

**Success Criteria:**
- ✅ Data received < 3 seconds
- ✅ Hop count = 2
- ✅ TTL: 7→6→5
- ✅ Full path logged

---

#### Test 5: LPN Friendship

**Objective:** Establish LPN-Friend relationship

**Procedure:**
```bash
# On Node 1 (LPN)
lpn enable
lpn establish 0x0002

# Expected:
[LPN] Sending Friend Request
[LPN] Friend Offer received from 0x0002
[LPN] Friendship established
[LPN] Poll interval: 10000ms
```

**Success Criteria:**
- ✅ Friendship established < 5 seconds
- ✅ Friend caches messages
- ✅ LPN polls every 10s

---

#### Test 6: Message Caching

**Objective:** Verify Friend caches messages for sleeping LPN

**Timeline:**
```
T=0s    : Node 1 enters deep sleep
T=5s    : Gateway sends message to 0x0001
T=5s    : Node 2 caches message
T=10s   : Node 1 wakes, polls Node 2
T=10.1s : Node 2 delivers cached message
```

**Success Criteria:**
- ✅ Message cached by Friend
- ✅ Message delivered on poll
- ✅ No message loss

---

#### Test 7: Network Resilience

**Objective:** Test mesh self-healing

**Procedure:**
1. Establish normal operation
2. Power off Node 2 (simulate failure)
3. Wait 30-60 seconds
4. Verify alternate route used

**Expected:**
```
[MESH] Node 0x0002 timeout
[MESH] Recalculating routes...
[MESH] New route: 0x0001 → 0x0004 → 0x0003 → Gateway
```

**Success Criteria:**
- ✅ Failure detected < 60s
- ✅ Alternate route established
- ✅ Data continues to flow

---

### Data Collection

#### Capturing Logs

```bash
# Terminal 1: Node 1 (Source)
platformio device monitor -p /dev/cu.usbserial-NODE1 | tee node1_log.txt

# Terminal 2: Node 2 (Relay)
platformio device monitor -p /dev/cu.usbserial-NODE2 | tee node2_log.txt

# Terminal 3: Gateway
platformio device monitor -p /dev/cu.usbserial-GATEWAY | tee gateway_log.txt
```

#### Log Analysis

```bash
# Analyze Gateway logs
python tools/analyze_mesh_logs.py gateway_log.txt
```

**Output:**
```
📊 MESSAGE DELIVERY ANALYSIS
  Total messages: 24
  Packet delivery rate: 100%

⏱️ NETWORK LATENCY ANALYSIS
  Average delay: 150ms
  Max delay: 280ms

🔗 HOP COUNT DISTRIBUTION
  0 hops: 4 messages (16.7%)
  1 hop:  12 messages (50.0%)
  2 hops: 8 messages (33.3%)

🌡️ SENSOR DATA ANALYSIS
  Temperature: 22.3°C (avg)
  Humidity: 65.2% (avg)
  Optimal conditions: 95.8% of time

✅ Outputs: mesh_analysis.png, mesh_data.csv
```

---

## 🔋 Power Consumption Testing

### Setup

**Equipment:**
- Multimeter or power analyzer (e.g., Nordic PPK2)
- 2000mAh LiPo battery
- Node configured as LPN

### Measurement Points

```
┌─────────────┐
│   Battery   │
│   2000mAh   │
└──────┬──────┘
       │
   [Ammeter]  ← Measure here
       │
   ┌───┴───┐
   │ESP32  │
   │-C3    │
   └───────┘
```

### Test Procedure

1. **Configure Node for Low Power**
   ```c
   // Enable LPN mode
   config.enable_lpn = true;
   config.lpn_poll_interval_ms = 10000;
   
   // 5-minute publish interval
   config.publish_interval_ms = 300000;
   ```

2. **Measure Current**
   ```bash
   # Monitor for 1 hour minimum
   # Record:
   - Deep sleep current
   - Wake-up current
   - Sensor read current
   - BLE TX current
   ```

3. **Calculate Average**
   ```
   Per 5-minute cycle:
   - Sleep: 299.85s @ 10 µA  = 0.833 µAh
   - Active: 0.15s @ 10 mA   = 0.417 µAh
   Total: ~0.94 mAh per cycle
   
   Cycles per day: 288
   Daily consumption: 270.7 mAh
   
   Battery life:
   2000mAh / 270.7 mAh/day = 7.4 days × 30 = 222 days (~7 months)
   ```

### Expected Results

| State | Current | Duration per Cycle |
|-------|---------|-------------------|
| **Deep Sleep** | 10 µA | 299.85 s |
| **Wake Up** | 5 mA | 0.05 s |
| **Sensor Read** | 5 mA | 0.05 s |
| **BLE TX** | 15 mA | 0.05 s |
| **LPN Poll** | 8 mA | 0.05 s |

**Success Criteria:**
- ✅ Sleep current < 20 µA
- ✅ Average current < 1.5 mA
- ✅ Battery life > 6 months
- ✅ No unexpected wake-ups

### Optimization Tips

1. **Reduce Active Time**
   - Minimize sensor warm-up time
   - Batch operations
   - Use DMA for I2C

2. **Deeper Sleep**
   - Disable unused peripherals
   - Use RTC memory for state
   - Minimize wake-up sources

3. **Adaptive Publishing**
   - Publish only on changes
   - Increase interval when stable
   - Use LPN friendship

---

## 📊 Test Analysis

### Automated Analysis Tool

**Usage:**
```bash
python tools/analyze_mesh_logs.py gateway_log.txt
```

**Features:**
- Message delivery rate calculation
- Network latency analysis
- Hop count distribution
- Sensor data validation
- Error detection
- Automatic plot generation
- CSV export

### Manual Analysis

**Key Metrics:**

1. **Packet Delivery Rate**
   ```
   PDR = (Messages Received / Messages Sent) × 100%
   Target: >99%
   ```

2. **Average Latency**
   ```
   Latency = Time(Gateway Receipt) - Time(Node Publish)
   Target: <300ms
   ```

3. **Hop Efficiency**
   ```
   Efficiency = Direct Path Hops / Actual Hops
   Target: >80%
   ```

4. **Sensor Data Quality**
   ```
   - Temperature accuracy: ±0.3°C
   - Humidity accuracy: ±2%
   - Update frequency: Every 5 minutes
   ```

### Test Report

Use the checklist: `HARDWARE_TEST_CHECKLIST.md`

**Sections:**
- Hardware setup
- Provisioning status
- Test results (10 tests)
- Performance metrics
- Issues encountered
- Recommendations

---

## 🐛 Troubleshooting

### Common Issues

**Provisioning Fails**
```
Symptoms: Node doesn't appear in nRF Mesh app
Solutions:
1. Check BLE is enabled
2. Move closer (<2m)
3. Reset device
4. Check device UUID is unique
```

**Messages Not Relaying**
```
Symptoms: Direct works, relay doesn't
Solutions:
1. Verify Friend feature enabled
2. Check TTL ≥ hops + 1
3. Verify group subscription (0xC000)
4. Check relay nodes powered
```

**High Packet Loss**
```
Symptoms: <95% delivery rate
Solutions:
1. Reduce node spacing
2. Add more relay nodes
3. Check RF interference
4. Increase transmit power
```

**LPN Can't Sleep**
```
Symptoms: High power consumption
Solutions:
1. Verify friendship established
2. Check deep sleep enabled
3. Review wake-up sources
4. Disable debug logging
```

---

## 📚 Additional Resources

**Documentation:**
- `docs/BLE_MESH_IMPLEMENTATION.md` - Implementation details
- `HARDWARE_TEST_CHECKLIST.md` - Printable checklist
- `README.md` - Project overview

**Tools:**
- `tools/analyze_mesh_logs.py` - Log analyzer
- `run_tests.sh` - Test runner
- nRF Mesh App - Provisioning

**References:**
- [ESP-BLE-MESH Docs](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/esp-ble-mesh/)
- [Bluetooth Mesh Spec](https://www.bluetooth.com/specifications/specs/mesh-protocol/)

---

**Document Version**: 2.0  
**Last Updated**: November 4, 2025  
**Status**: Complete and Ready for Use
