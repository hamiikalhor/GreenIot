# 🔌 Hardware Design

**Status**: ⏳ Validation Pending (80% Complete)  
**Last Updated**: November 4, 2025

---

## 📚 Directory Structure

```
02_Hardware/
├── Altium/                    # PCB design files
│   ├── Schematics            # Circuit schematics
│   ├── PCB Layouts           # PCB layout designs
│   ├── Libraries             # Component libraries
│   └── Design Rules          # Design rule checks
│
├── Simulation/                # Circuit simulations
│   ├── SPICE Models          # Component models
│   ├── Power Analysis        # Power consumption
│   └── Signal Integrity      # SI/PI analysis
│
├── Hardware_Review_Reports/   # Design reviews
│   ├── Review Documentation
│   ├── Design Verification
│   └── Change Logs
│
└── Sent_For_Print/           # Manufacturing files
    ├── Gerber Files          # Ready for production
    └── Final Designs         # Approved designs
```

---

## 🎯 Hardware Overview

### System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                   GreenIoT Sensor Node                      │
└─────────────────────────────────────────────────────────────┘

  Battery (3.7V)                USB Type-C (5V)
       │                              │
       │                              │
       ├──────────┐          ┌────────┴────────┐
       │  Power   │          │   MCP73831      │
       │  Switch  │          │   Charger       │
       └────┬─────┘          └────────┬────────┘
            │                         │
            └────────┬────────────────┘
                     │
              ┌──────┴──────┐
              │  MCP1700    │ Ultra-Low Power LDO
              │  (1.6μA)    │
              └──────┬──────┘
                     │ 3.3V
        ┌────────────┼────────────┐
        │            │            │
   ┌────▼────┐  ┌───▼────┐  ┌───▼────┐
   │ ESP32-C3│  │ SHT31  │  │  LED   │
   │  MCU    │──┤ Sensor │  │ Status │
   │ BLE 5.0 │  │ I2C    │  └────────┘
   └─────────┘  └────────┘
        │
     GPIO/ADC
        │
   Battery Monitor
```

---

## 🔧 Key Components

### Main ICs

| Component | Part Number | Package | Function |
|-----------|-------------|---------|----------|
| **MCU** | ESP32-C3-DevKitM-1 | Module | Main processor with BLE 5.0 |
| **Sensor** | SHT31-DIS-B | DFN-8 | Temperature & Humidity |
| **LDO** | MCP1700-3302E | SOT-23 | Ultra-low power regulator |
| **Charger** | MCP73831T-2ACI | SOT-23-5 | Li-Ion battery charger |

### Power Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Input Voltage** | 3.0V - 4.2V | Li-Ion battery range |
| **Regulated Output** | 3.3V ±2% | System power rail |
| **LDO Quiescent Current** | 1.6μA | Critical for battery life |
| **Sleep Current** | ~12μA | Total system |
| **Active Current** | ~100mA | During measurement |
| **Charge Current** | 500mA | Programmable |

---

## 📐 Pin Configuration

### ESP32-C3 Connections

```
┌─────────────────────────────────────────────────────────┐
│                    ESP32-C3 Pinout                      │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  GPIO8  ──────► SDA (I2C) ──────────► SHT31           │
│  GPIO9  ──────► SCL (I2C) ──────────► SHT31           │
│  GPIO10 ──────► Sensor Power Control                   │
│  ADC1_CH0 ────► Battery Voltage Monitor                │
│  GPIO0  ──────► Boot/Program Button                    │
│  EN     ──────► Reset Button                           │
│  GPIO2  ──────► Status LED                             │
│                                                         │
│  3.3V   ──────► System Power                           │
│  GND    ──────► Ground                                  │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### I2C Bus Configuration

| Signal | GPIO | Pull-up | Notes |
|--------|------|---------|-------|
| **SDA** | GPIO8 | 10kΩ to 3.3V | I2C data line |
| **SCL** | GPIO9 | 10kΩ to 3.3V | I2C clock line |
| **Speed** | - | 100kHz | Standard mode |
| **Address** | - | 0x44 | SHT31 sensor |

---

## 📋 Schematic Overview

### Power Supply Stage
- **Input**: 18650 Li-Ion battery (3.0V - 4.2V)
- **Charging**: MCP73831 with USB Type-C input
- **Regulation**: MCP1700-3302E for 3.3V rail
- **Protection**: Reverse polarity diode, over-voltage protection

### Sensor Interface
- **I2C Communication**: SHT31 sensor @ 0x44
- **Power Control**: GPIO10 controls sensor power via P-MOSFET
- **Pull-ups**: 10kΩ resistors for SDA/SCL

### Battery Monitoring
- **ADC**: ESP32-C3 ADC1_CH0
- **Voltage Divider**: 10kΩ / 10kΩ (50% divider)
- **Range**: 0V - 4.2V battery → 0V - 2.1V ADC

### User Interface
- **Status LED**: Green LED on GPIO2 with 1kΩ resistor
- **Reset Button**: Tactile switch to EN pin
- **Power Switch**: SPDT slide switch on battery line

---

## 🔌 PCB Specifications

### Physical Characteristics
- **Dimensions**: 50mm × 40mm
- **Layers**: 2-layer FR4
- **Thickness**: 1.6mm standard
- **Copper Weight**: 1 oz (35μm)

### Manufacturing Specifications
- **Min Trace Width**: 0.2mm (8 mil)
- **Min Spacing**: 0.2mm (8 mil)
- **Min Drill**: 0.3mm
- **Via Size**: 0.6mm hole / 1.0mm pad
- **Surface Finish**: ENIG (lead-free)
- **Solder Mask**: Green
- **Silkscreen**: White

### Design Rules
- **Trace Width** (3.3V): 0.4mm (adequate for 500mA)
- **Trace Width** (Signal): 0.25mm minimum
- **Ground Pour**: Both layers
- **Thermal Relief**: 4 spokes, 0.3mm width

---

## 🧪 Simulation & Analysis

### Power Analysis
- **Sleep Mode**: 12μA verified
- **Active Mode**: 80-100mA verified
- **Efficiency**: >85% @ 100mA load
- **Battery Life**: 5-7 years calculated

### Signal Integrity
- **I2C Rise Time**: <300ns verified
- **I2C Fall Time**: <300ns verified
- **Clock Frequency**: 100kHz stable
- **EMI**: Acceptable levels

### Thermal Analysis
- **Operating Temp**: 0°C to 50°C
- **Max Junction Temp**: <85°C
- **Heat Dissipation**: Passive cooling adequate
- **Thermal Management**: No heatsink required

---

## 📊 Design Reviews

### Review Status

| Review Phase | Date | Status | Notes |
|--------------|------|--------|-------|
| **Concept Review** | Oct 2025 | ✅ Approved | Architecture validated |
| **Schematic Review** | Oct 2025 | ✅ Approved | Pin assignments verified |
| **PCB Layout Review** | Nov 2025 | ⏳ Pending | In progress |
| **Design Validation** | Nov 2025 | 📋 Planned | Post-prototype |

### Change Log
- **v1.0**: Initial design with AMS1117 (rejected)
- **v1.1**: Updated to MCP1700 for ultra-low power
- **v1.2**: Changed charger from TP4056 to MCP73831
- **v1.3**: Current version (pending validation)

---

## 🔗 Related Documentation

- [📋 Requirements](../01_Requirements/) - Electrical specifications
- [💻 Firmware](../03_Firmware/GreenIot/) - Software for hardware control
- [🏭 Production](../06_Production/) - BOM and manufacturing files
- [🧪 Testing](../05_Test_and_Validation/) - Hardware test procedures
- [📚 Main README](../README.md) - Project overview

---

## 📞 Support

For hardware design questions:
- **GitHub Issues**: [Create an issue](https://github.com/hamiikalhor/GreenIot/issues)
- **Component Selection**: See [Component Selection Notes](../06_Production/BOM/COMPONENT_SELECTION_NOTES.md)

---

**[⬅️ Back to Main README](../README.md)**

