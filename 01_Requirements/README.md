# 📋 Requirements & Specifications

**Status**: ✅ Complete  
**Last Updated**: November 4, 2025

---

## 📚 Contents

This directory contains all system requirements and specifications for the GreenIoT Vertical Farming environmental monitoring system.

### 📄 Documents

| Document | Description | Format | Status |
|----------|-------------|--------|--------|
| **Product_Specifications.docx** | Complete product specification document | Word | ✅ Complete |
| **Electrical_Requirements.xlsx** | Electrical design specifications and requirements | Excel | ✅ Complete |
| **Mechanical_Requirements.xlsx** | Mechanical design constraints and specifications | Excel | ✅ Complete |
| **Safety_and_Compliance_Standards.docx** | Safety standards and regulatory compliance | Word | ✅ Complete |
| **Functional_Block_Diagram.vsdx** | System architecture and block diagrams | Visio | ✅ Complete |

---

## 🎯 Key Specifications

### System Overview
- **Application**: Vertical farming environmental monitoring
- **Target Crop**: Basil (Ocimum basilicum)
- **Deployment**: Indoor agricultural facilities
- **Network**: BLE Mesh 5.0 multi-node system

### Hardware Specifications
- **MCU**: ESP32-C3 (RISC-V @ 160MHz)
- **Sensor**: SHT31-DIS-B (Temperature & Humidity)
- **Power**: 18650 Li-Ion 3.7V 3000mAh
- **Battery Life**: 5-7 years
- **Communication**: BLE Mesh 5.0

### Performance Requirements
- **Temperature Accuracy**: ±0.3°C
- **Humidity Accuracy**: ±2% RH
- **Measurement Interval**: 15 minutes
- **Operating Temperature**: 0°C to 50°C
- **Storage Temperature**: -20°C to 60°C

### Power Specifications
- **Sleep Current**: ~12μA
- **Active Current**: ~100mA (2 seconds)
- **Average Current**: ~15μA
- **LDO**: MCP1700-3302E (1.6μA quiescent)
- **Charger**: MCP73831 (500mA)

---

## 📊 Electrical Requirements Summary

### Pin Configuration (ESP32-C3)

```
GPIO8   → SDA (I2C)
GPIO9   → SCL (I2C)
GPIO10  → Sensor Power Control
ADC1_CH0 → Battery Voltage Monitoring
3.3V    → System Power
GND     → Ground
```

### I2C Configuration
- **SDA**: GPIO8 with 10kΩ pull-up
- **SCL**: GPIO9 with 10kΩ pull-up
- **Clock Speed**: 100kHz (standard mode)
- **Sensor Address**: 0x44 (SHT31)

### Power Supply
- **Input**: 3.7V Li-Ion battery
- **Regulated**: 3.3V via MCP1700
- **Charging**: USB Type-C 5V via MCP73831
- **Protection**: Over-voltage, under-voltage, over-current

---

## 🔧 Mechanical Requirements Summary

### Physical Dimensions
- **PCB Size**: 50mm × 40mm (2-layer FR4)
- **Enclosure**: 60mm × 50mm × 25mm
- **Material**: ABS plastic (weatherproof)
- **Weight**: ~80g (with battery)

### Environmental Ratings
- **IP Rating**: IP54 (dust and splash resistant)
- **Operating Humidity**: 0-95% RH (non-condensing)
- **Shock Resistance**: IEC 60068-2-27
- **Vibration**: IEC 60068-2-6

### Mounting
- **Type**: Wall mount or hanging
- **Hardware**: M2.5 screws and standoffs
- **Orientation**: Any (optimal: vertical)

---

## ✅ Compliance & Safety

### Standards
- **CE Marking**: EN 301 489-1, EN 301 489-17
- **FCC**: Part 15 Subpart C (Radio)
- **RoHS**: Directive 2011/65/EU
- **REACH**: EC 1907/2006

### Safety Features
- **Battery Protection**: Over-charge, over-discharge
- **Thermal Management**: Automatic throttling
- **Reverse Polarity**: Schottky diode protection
- **ESD Protection**: TVS diodes on exposed pins

---

## 📈 Performance Targets

### Basil Cultivation Optimal Ranges
- **Temperature**: 20-25°C (68-77°F)
- **Humidity**: 40-60% RH
- **Light**: 12-16 hours/day (not monitored)
- **Monitoring Frequency**: Every 15 minutes

### System Reliability
- **MTBF**: >50,000 hours
- **Data Accuracy**: 99.5% valid readings
- **Network Reliability**: 99% uptime
- **Battery Life**: 5-7 years typical use

---

## 🔗 Related Documentation

- [📦 Hardware Design](../02_Hardware/) - PCB schematics and layouts
- [💻 Firmware](../03_Firmware/GreenIot/) - Software implementation
- [🧪 Testing](../05_Test_and_Validation/) - Validation reports
- [🏭 Production](../06_Production/) - Manufacturing files and BOM
- [📚 Main README](../README.md) - Project overview

---

## 📞 Contact

For questions about requirements or specifications:
- **GitHub Issues**: [Create an issue](https://github.com/hamiikalhor/GreenIot/issues)
- **Project Team**: GreenIoT Development Team

---

**[⬅️ Back to Main README](../README.md)**

