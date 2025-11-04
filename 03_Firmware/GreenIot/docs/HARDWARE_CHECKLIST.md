# ✅ Hardware Test Checklist

**Print-friendly checklist for GreenIoT BLE Mesh hardware validation**

---

## 📋 Pre-Test Preparation

### Hardware Procurement
- [ ] 3-5× ESP32-C3-DevKitM-1 boards
- [ ] 2× SHT31 temperature/humidity sensors
- [ ] 3-5× USB cables (data-capable)
- [ ] 2× 2000mAh LiPo batteries
- [ ] 2× Battery connectors (JST-PH 2.0)
- [ ] Breadboards or prototyping PCBs
- [ ] Jumper wires

### Software Setup
- [ ] PlatformIO installed
- [ ] ESP-IDF framework configured
- [ ] nRF Mesh App downloaded
- [ ] Serial terminal ready
- [ ] Python 3.x + matplotlib installed

---

## 🔧 Hardware Assembly

### Node 1 & 5 (Sensor Nodes - LPN)
- [ ] ESP32-C3 mounted
- [ ] SHT31 connected (VCC, GND, SDA→GPIO8, SCL→GPIO9)
- [ ] Pull-up resistors (4.7kΩ) on I2C lines
- [ ] Battery connector wired
- [ ] Power verified (3.3V on VCC)

### Node 2, 3, 4 (Friend Nodes)
- [ ] ESP32-C3 mounted
- [ ] USB power connected
- [ ] Status LED (optional)

### Gateway
- [ ] ESP32-C3 or RPi setup
- [ ] USB to PC working
- [ ] Serial terminal at 115200 baud

---

## 📡 Firmware Flashing

### All Nodes
```bash
platformio run -e esp32-c3-devkitm-1 -t upload --upload-port /dev/cu.usbserial-XXXXX
```

- [ ] Node 1 flashed, boots, detects SHT31
- [ ] Node 2 flashed, boots
- [ ] Node 3 flashed, boots
- [ ] Node 4 (Gateway) flashed, boots
- [ ] Node 5 flashed, boots, detects SHT31

---

## 🌐 Network Provisioning

### Using nRF Mesh App
- [ ] Network created: "GreenIoT-Basil-Farm"
- [ ] Node 1 provisioned (0x0001)
- [ ] Node 2 provisioned (0x0002)
- [ ] Node 3 provisioned (0x0003)
- [ ] Gateway provisioned (0x0004)
- [ ] Node 5 provisioned (0x0005)
- [ ] All subscribed to group 0xC000
- [ ] TTL set to 7

---

## 🧪 Test Execution

### Test 1: Basic Connectivity
- [ ] All nodes respond to ping
- [ ] Response time < 100ms

**Result:** ⬜ PASS / ⬜ FAIL

### Test 2: Direct Communication
- [ ] Node 1 publishes sensor data
- [ ] Gateway receives within 1 second
- [ ] Hop count = 0

**Result:** ⬜ PASS / ⬜ FAIL

### Test 3: Single-Hop Relay
- [ ] Message relays through Node 2
- [ ] Gateway receives within 2 seconds
- [ ] Hop count = 1

**Result:** ⬜ PASS / ⬜ FAIL

### Test 4: Multi-Hop Relay
- [ ] Message relays through 2 nodes
- [ ] Gateway receives within 3 seconds
- [ ] TTL decrements correctly

**Result:** ⬜ PASS / ⬜ FAIL

### Test 5: LPN Friendship
- [ ] Friendship established < 5 seconds
- [ ] Poll interval 10000ms
- [ ] Messages cached and delivered

**Result:** ⬜ PASS / ⬜ FAIL

### Test 6: Power Consumption
- [ ] Deep sleep: _____ µA
- [ ] Active: _____ mA
- [ ] Average < 1.5 mA

**Result:** ⬜ PASS / ⬜ FAIL

---

## 📝 Test Summary

**Date:** _______________  
**Tester:** _______________  
**Tests Passed:** ____ / 6  
**Overall:** ⬜ PASS / ⬜ FAIL  
**Ready for Production:** ⬜ YES / ⬜ NO

**Signature:** _______________ **Date:** _______________

---

For complete procedures, see `docs/TESTING.md`

