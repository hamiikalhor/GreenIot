# ✅ Hardware Test Checklist

## 📋 Pre-Test Preparation

### Hardware Procurement
- [ ] 3-5× ESP32-C3-DevKitM-1 boards
- [ ] 2× SHT31 temperature/humidity sensors
- [ ] 3-5× USB cables (data-capable, not charge-only)
- [ ] 2× 2000mAh LiPo batteries
- [ ] 2× Battery holders/connectors (JST-PH 2.0)
- [ ] Breadboards or prototyping PCBs
- [ ] Jumper wires (male-female, female-female)

### Software Setup
- [ ] PlatformIO IDE installed and working
- [ ] ESP-IDF framework configured
- [ ] nRF Mesh App downloaded (iOS/Android)
- [ ] Serial terminal (PuTTY/screen/minicom)
- [ ] Python 3.x installed
- [ ] Matplotlib installed (`pip install matplotlib pandas`)

### Firmware Preparation
- [ ] Latest firmware built successfully
- [ ] USB drivers installed (CP210x/CH340)
- [ ] Serial ports identified (`ls /dev/cu.*` or `ls /dev/ttyUSB*`)
- [ ] Firmware flashed to at least one device (test)

---

## 🔧 Hardware Assembly

### Node 1 & 5 (Sensor Nodes - LPN)
- [ ] ESP32-C3 mounted on breadboard
- [ ] SHT31 sensor connected:
  - [ ] VCC → 3.3V
  - [ ] GND → GND
  - [ ] SDA → GPIO 8 (default I2C SDA)
  - [ ] SCL → GPIO 9 (default I2C SCL)
- [ ] Pull-up resistors (4.7kΩ) on SDA/SCL (if needed)
- [ ] Battery connector wired (VIN/GND)
- [ ] Power switch installed (optional)
- [ ] LED indicator connected (optional, GPIO 2)

### Node 2, 3, 4 (Friend Nodes - Relays)
- [ ] ESP32-C3 mounted on breadboard
- [ ] USB power connected
- [ ] Status LED connected (optional, GPIO 2)
- [ ] No sensors required (relay only)

### Gateway (Provisioner)
- [ ] ESP32-C3 or Raspberry Pi setup
- [ ] USB connection to PC verified
- [ ] Serial terminal working at 115200 baud
- [ ] (If using RPi) nRF52840 dongle connected

---

## 📡 Firmware Flashing

### Node 1 & 5 (LPN)
```bash
cd /Users/hamidrezakalhor/Desktop/GreenIot/03_Firmware/GreenIot

# Flash Node 1
platformio run -e esp32-c3-devkitm-1 -t upload --upload-port /dev/cu.usbserial-XXXXX

# Verify Node 1
platformio device monitor -p /dev/cu.usbserial-XXXXX
```
- [ ] Node 1 flashed successfully
- [ ] Node 1 boots and shows startup message
- [ ] Node 1 detects SHT31 sensor (I2C scan)
- [ ] Repeat for Node 5
- [ ] Node 5 flashed successfully
- [ ] Node 5 boots and shows startup message
- [ ] Node 5 detects SHT31 sensor

### Node 2, 3, 4 (Friend)
```bash
# Flash each Friend node
platformio run -e esp32-c3-devkitm-1 -t upload --upload-port /dev/cu.usbserial-YYYYY
```
- [ ] Node 2 flashed successfully
- [ ] Node 2 boots and shows startup message
- [ ] Node 3 flashed successfully
- [ ] Node 3 boots and shows startup message
- [ ] Node 4 flashed successfully
- [ ] Node 4 boots and shows startup message

### Gateway (Provisioner)
```bash
# Flash provisioner firmware
# (or use nRF Mesh app on mobile)
```
- [ ] Gateway flashed successfully
- [ ] Gateway boots and shows startup message
- [ ] Serial monitor working
- [ ] BLE provisioning enabled

---

## 🌐 Network Provisioning

### Using nRF Mesh App
- [ ] App installed and opened
- [ ] Network created: "GreenIoT-Basil-Farm"
- [ ] NetKey auto-generated
- [ ] AppKey added

### Provision Node 1 (LPN)
- [ ] Scan for unprovisioned devices
- [ ] Found "GreenIoT-Node-XXXX"
- [ ] Provisioning completed
- [ ] Unicast address assigned: 0x0001
- [ ] AppKey added
- [ ] Subscribed to group: 0xC000
- [ ] Publish address set: 0xC000
- [ ] TTL set: 7

### Provision Node 2 (Friend)
- [ ] Scan for unprovisioned devices
- [ ] Provisioning completed
- [ ] Unicast address assigned: 0x0002
- [ ] AppKey added
- [ ] Friend feature enabled
- [ ] Relay feature enabled

### Provision Node 3 (Friend)
- [ ] Scan for unprovisioned devices
- [ ] Provisioning completed
- [ ] Unicast address assigned: 0x0003
- [ ] AppKey added
- [ ] Friend feature enabled
- [ ] Relay feature enabled

### Provision Node 4 (Gateway)
- [ ] Provisioning completed
- [ ] Unicast address assigned: 0x0004
- [ ] AppKey added
- [ ] Subscribed to group: 0xC000

### Provision Node 5 (LPN)
- [ ] Provisioning completed
- [ ] Unicast address assigned: 0x0005
- [ ] AppKey added
- [ ] Subscribed to group: 0xC000
- [ ] Publish address set: 0xC000

---

## 🧪 Test Execution

### Test 1: Basic Connectivity
- [ ] All nodes powered on
- [ ] All nodes appear in nRF Mesh app
- [ ] Ping Node 1 → ACK received
- [ ] Ping Node 2 → ACK received
- [ ] Ping Node 3 → ACK received
- [ ] Ping Node 5 → ACK received
- [ ] Response time < 100ms for all

**Result:** ⬜ PASS / ⬜ FAIL  
**Notes:** _______________________________

### Test 2: Direct Communication
- [ ] Node 1 placed within 5m of Gateway
- [ ] Other nodes powered off
- [ ] Node 1 publishes sensor data
- [ ] Gateway receives data within 1 second
- [ ] Temperature value correct
- [ ] Humidity value correct
- [ ] Hop count = 0
- [ ] RSSI > -70 dBm

**Result:** ⬜ PASS / ⬜ FAIL  
**Notes:** _______________________________

### Test 3: Single-Hop Relay
- [ ] Node 1 moved >10m from Gateway
- [ ] Node 2 placed midway (5m from each)
- [ ] Node 1 publishes sensor data
- [ ] Node 2 relays message
- [ ] Gateway receives data within 2 seconds
- [ ] Hop count = 1
- [ ] Relay node logged correctly
- [ ] No data corruption

**Result:** ⬜ PASS / ⬜ FAIL  
**Notes:** _______________________________

### Test 4: Multi-Hop Relay
- [ ] Nodes arranged: Node1 → Node2 → Node3 → Gateway
- [ ] Node 1 publishes sensor data
- [ ] Message relays through Node 2
- [ ] Message relays through Node 3
- [ ] Gateway receives data within 3 seconds
- [ ] Hop count = 2
- [ ] Relay path tracked correctly
- [ ] TTL decrements properly (7→6→5)

**Result:** ⬜ PASS / ⬜ FAIL  
**Notes:** _______________________________

### Test 5: LPN Friendship
- [ ] Node 1 LPN mode enabled
- [ ] Node 2 Friend feature enabled
- [ ] Friendship request sent
- [ ] Friendship offer received
- [ ] Friendship established < 5 seconds
- [ ] Poll interval: 10000ms
- [ ] Friend caches messages
- [ ] LPN polls successfully

**Result:** ⬜ PASS / ⬜ FAIL  
**Notes:** _______________________________

### Test 6: Message Caching
- [ ] LPN friendship established (Node 1 ↔ Node 2)
- [ ] Node 1 enters deep sleep
- [ ] Gateway sends message to Node 1
- [ ] Node 2 caches message
- [ ] Node 1 wakes up after 10 seconds
- [ ] Node 1 polls Friend
- [ ] Cached message delivered
- [ ] No message loss

**Result:** ⬜ PASS / ⬜ FAIL  
**Notes:** _______________________________

### Test 7: End-to-End Sensor Data
- [ ] All nodes powered and provisioned
- [ ] Node 1 reads SHT31
- [ ] Temperature reading valid
- [ ] Humidity reading valid
- [ ] Data published to mesh
- [ ] Message relayed through mesh
- [ ] Gateway receives complete data
- [ ] End-to-end delay < 300ms
- [ ] Data integrity verified

**Result:** ⬜ PASS / ⬜ FAIL  
**Notes:** _______________________________

### Test 8: Network Resilience
- [ ] Normal mesh operation established
- [ ] Node 2 powered off (simulate failure)
- [ ] Failure detected within 60 seconds
- [ ] Alternate route established
- [ ] Data continues to flow via Node 4
- [ ] No manual intervention required
- [ ] Recovery automatic

**Result:** ⬜ PASS / ⬜ FAIL  
**Notes:** _______________________________

### Test 9: Power Consumption
- [ ] Multimeter connected to Node 1
- [ ] Deep sleep current measured: _____ µA
- [ ] Active current measured: _____ mA
- [ ] Transmit current measured: _____ mA
- [ ] Average current < 1.5 mA
- [ ] Sleep current < 20 µA
- [ ] Projected battery life > 6 months

**Result:** ⬜ PASS / ⬜ FAIL  
**Measurements:**
- Deep sleep: _____ µA
- Active: _____ mA
- Transmit: _____ mA
- Average: _____ mA

### Test 10: Load Testing
- [ ] All 5 nodes active
- [ ] Nodes 1 & 5 publishing (5 min interval, offset)
- [ ] Test duration: 30 minutes
- [ ] All messages received
- [ ] Packet delivery rate > 99%
- [ ] Average latency < 300ms
- [ ] No node crashes
- [ ] Network remains stable

**Result:** ⬜ PASS / ⬜ FAIL  
**Metrics:**
- Messages expected: _____
- Messages received: _____
- Delivery rate: _____%
- Average latency: _____ ms

---

## 📊 Data Collection

### Serial Logs Captured
- [ ] Gateway log saved: `gateway_log.txt`
- [ ] Node 1 log saved: `node1_log.txt`
- [ ] Node 2 log saved: `node2_log.txt`
- [ ] Test duration: _____ minutes
- [ ] Log file sizes: _____ KB

### Analysis Completed
```bash
python tools/analyze_mesh_logs.py gateway_log.txt
```
- [ ] Log analysis script run successfully
- [ ] Plots generated: `mesh_analysis.png`
- [ ] CSV exported: `mesh_data.csv`
- [ ] Summary statistics calculated

---

## 📝 Post-Test

### Hardware Condition
- [ ] All nodes powered off safely
- [ ] No physical damage observed
- [ ] Sensors still functional
- [ ] Batteries charged/replaced
- [ ] Hardware stored properly

### Documentation
- [ ] Test report completed
- [ ] Photos taken of setup
- [ ] Issues documented
- [ ] Recommendations noted
- [ ] Results shared with team

### Next Steps
- [ ] Deploy to production (if tests passed)
- [ ] Fix identified issues (if tests failed)
- [ ] Schedule follow-up testing
- [ ] Update firmware if needed
- [ ] Plan scaling deployment

---

## 📋 Test Summary

**Test Date:** _______________  
**Tester:** _______________  
**Firmware Version:** _______________  
**Hardware Version:** _______________

**Overall Result:** ⬜ PASS / ⬜ FAIL

**Tests Passed:** ____ / 10

**Critical Issues:** _______________________________________________

**Recommendations:** _______________________________________________

**Ready for Production:** ⬜ YES / ⬜ NO

**Signed:** _______________ **Date:** _______________

---

<div align="center">

## ⏳ Status: Ready for Hardware Testing

**Print this checklist and use it during testing**

**Estimated Time:** 4-6 hours

</div>

