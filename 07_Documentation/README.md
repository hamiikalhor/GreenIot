# 📚 User Documentation

**Status**: ⏳ Being Prepared (30% Complete)  
**Last Updated**: November 4, 2025

---

## 📚 Directory Structure

```
07_Documentation/
├── User_Manual/             # End-user guides
│   ├── Getting Started
│   ├── Operation Instructions
│   ├── Troubleshooting
│   └── FAQ
│
├── Installation_Guide/      # Setup instructions
│   ├── Hardware Assembly
│   ├── Software Installation
│   ├── Network Configuration
│   └── Initial Setup
│
├── Maintenance_Guide/       # Service manual
│   ├── Maintenance Procedures
│   ├── Calibration Instructions
│   ├── Replacement Procedures
│   └── Service Schedules
│
├── Datasheets/             # Component datasheets
│   ├── ESP32-C3 Datasheet
│   ├── SHT31 Sensor Datasheet
│   ├── MCP1700 LDO Datasheet
│   └── Component References
│
└── Marketing_Materials/     # Promotional content
    ├── Product Brochures
    ├── Technical Briefs
    ├── Presentations
    └── Specification Sheets
```

---

## 📖 Available Documentation

### Quick Reference

| Document | Type | Status | Audience |
|----------|------|--------|----------|
| **User Manual** | Guide | 📋 Planned | End Users |
| **Installation Guide** | Guide | 📋 Planned | Installers |
| **Maintenance Guide** | Manual | 📋 Planned | Technicians |
| **Technical Brief** | Reference | 📋 Planned | Technical |
| **Quick Start Guide** | Guide | ✅ In README | All Users |

---

## 🚀 Quick Start (Embedded in Main README)

For immediate getting started information, see the [Quick Start section](../README.md#-quick-start) in the main README.

### 5-Minute Setup

```bash
# 1. Clone repository
git clone <repository-url>
cd GreenIot

# 2. Navigate to firmware
cd 03_Firmware/GreenIot

# 3. Build
pio run -e esp32-c3-devkitm-1

# 4. Upload
pio run --target upload

# 5. Monitor
pio device monitor
```

---

## 📋 User Manual (Planned)

### Contents

#### Chapter 1: Introduction
- System overview
- Key features
- Applications
- Safety warnings

#### Chapter 2: Getting Started
- Unboxing and components
- Initial setup
- First boot
- LED indicators

#### Chapter 3: Operation
- Normal operation mode
- Sensor readings interpretation
- Battery management
- Troubleshooting

#### Chapter 4: Maintenance
- Battery replacement
- Sensor cleaning
- Enclosure inspection
- Firmware updates

#### Chapter 5: Specifications
- Technical specifications
- Performance characteristics
- Environmental ratings
- Certifications

---

## 🔧 Installation Guide (Planned)

### Hardware Installation

1. **Unboxing**
   - Verify contents
   - Inspect for damage
   - Check accessories

2. **Mounting**
   - Select installation location
   - Mount using provided hardware
   - Ensure proper orientation

3. **Initial Configuration**
   - Insert battery
   - Power on device
   - Verify LED status
   - Check sensor readings

### Software Setup

1. **Prerequisites**
   - PlatformIO installation
   - USB-C cable
   - PC with serial terminal

2. **Firmware Upload**
   - Connect via USB-C
   - Build firmware
   - Upload to device
   - Verify operation

3. **BLE Mesh Configuration**
   - Provision device to mesh
   - Set node address
   - Configure publish/subscribe
   - Test connectivity

---

## 🛠️ Maintenance Guide (Planned)

### Regular Maintenance Schedule

| Task | Frequency | Procedure |
|------|-----------|-----------|
| **Visual Inspection** | Monthly | Check enclosure integrity |
| **Sensor Cleaning** | Quarterly | Clean dust from sensor |
| **Battery Check** | Semi-Annual | Verify battery voltage |
| **Calibration** | Annual | Verify sensor accuracy |
| **Firmware Update** | As Needed | Update to latest version |

### Troubleshooting

#### Device Won't Power On
```
Possible Causes:
□ Battery discharged
□ Power switch off
□ Battery connection loose

Solutions:
1. Charge battery via USB-C
2. Check power switch position
3. Verify battery connections
4. Measure battery voltage
```

#### Sensor Readings Incorrect
```
Possible Causes:
□ Sensor dirty or damaged
□ Calibration drift
□ Environmental interference

Solutions:
1. Clean sensor with soft brush
2. Perform calibration procedure
3. Check sensor mounting
4. Verify reference conditions
```

#### BLE Mesh Not Connecting
```
Possible Causes:
□ Node not provisioned
□ Out of mesh range
□ Network congestion

Solutions:
1. Re-provision device
2. Move closer to other nodes
3. Check mesh network status
4. Reset and reconfigure
```

---

## 📊 Datasheets

### Core Component Datasheets

| Component | Manufacturer | Document | Link |
|-----------|--------------|----------|------|
| **ESP32-C3** | Espressif | Technical Reference Manual | [View PDF](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf) |
| **SHT31** | Sensirion | Sensor Datasheet | [View PDF](https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf) |
| **MCP1700** | Microchip | LDO Datasheet | [View PDF](https://ww1.microchip.com/downloads/en/DeviceDoc/20001826E.pdf) |
| **MCP73831** | Microchip | Charger Datasheet | [View PDF](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP73831-Family-Data-Sheet-DS20001984H.pdf) |

### Additional References

- **BLE Mesh Specification**: [Bluetooth SIG](https://www.bluetooth.com/specifications/specs/mesh-profile-1-0-1/)
- **PlatformIO Documentation**: [docs.platformio.org](https://docs.platformio.org/)
- **Arduino-ESP32 Guide**: [GitHub](https://github.com/espressif/arduino-esp32)

---

## 📈 Marketing Materials (Planned)

### Product Brief

**GreenIoT Environmental Monitoring System**

Transform your vertical farm with precision environmental monitoring. Our BLE Mesh-enabled sensors provide real-time temperature and humidity data optimized for basil cultivation.

**Key Benefits:**
- ✅ **Ultra-Long Battery Life**: 5-7 years on single 18650 cell
- ✅ **Precision Monitoring**: ±0.3°C temperature, ±2% RH humidity
- ✅ **Scalable Network**: BLE Mesh supports 100+ nodes
- ✅ **Easy Installation**: Wireless, battery-powered, plug-and-play
- ✅ **Cost-Effective**: Under $20 per node
- ✅ **Production Ready**: 28/28 tests passing, 92% coverage

**Target Applications:**
- Vertical farming facilities
- Greenhouse monitoring
- Indoor agriculture
- Research applications
- Smart agriculture

### Technical Specifications

```
┌─────────────────────────────────────────────────────────┐
│           GreenIoT Technical Specifications             │
├─────────────────────────────────────────────────────────┤
│ MCU:          ESP32-C3 RISC-V @ 160MHz                 │
│ Sensor:       SHT31-DIS-B (I2C)                         │
│ Network:      BLE Mesh 5.0                              │
│ Power:        18650 Li-Ion 3.7V 3000mAh                │
│ Battery Life: 5-7 years                                 │
│ Dimensions:   60 × 50 × 25 mm                           │
│ Weight:       ~80g with battery                         │
│ IP Rating:    IP54 (dust/splash resistant)             │
│ Operating:    0°C to 50°C, 0-95% RH                    │
└─────────────────────────────────────────────────────────┘
```

---

## 🔗 Related Documentation

- [📋 Requirements](../01_Requirements/) - System specifications
- [🔌 Hardware](../02_Hardware/) - PCB design
- [💻 Firmware](../03_Firmware/GreenIot/) - Software documentation
- [🧪 Testing](../05_Test_and_Validation/) - Test reports
- [🏭 Production](../06_Production/) - BOM and manufacturing
- [📚 Main README](../README.md) - Project overview

---

## 📞 Support

For documentation questions or contributions:
- **GitHub Issues**: [Create an issue](https://github.com/hamiikalhor/GreenIot/issues)
- **Discussions**: [GitHub Discussions](https://github.com/hamiikalhor/GreenIot/discussions)

---

**[⬅️ Back to Main README](../README.md)**

