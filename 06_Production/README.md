# 🏭 Production Files

**Status**: ⏳ Preparation Phase  
**Last Updated**: November 4, 2025

---

## 📚 Directory Structure

```
06_Production/
├── BOM/                      # Bill of Materials
│   ├── GreenIoT_BOM.csv               ✅ Complete
│   ├── GreenIoT_BOM_Simple.csv        ✅ Complete
│   ├── COMPONENT_SELECTION_NOTES.md   ✅ Complete
│   ├── PCB_BOM.xlsx                   📋 Legacy
│   └── Mechanical_BOM.xlsx            📋 Legacy
│
└── Manufacturing_Files/      # Production data
    ├── Assembly_Drawings/    # Visual assembly guides
    ├── Assembly_Guides/      # Step-by-step instructions
    ├── Pick_and_Place/       # SMT machine files
    ├── Raw_PCB/             # Gerber files
    ├── Panelization/        # Panel layouts
    ├── Test_Fixtures/       # Testing jigs
    └── Vendor_Info/         # Supplier contacts
```

---

## 📋 Bill of Materials (BOM)

### 📄 Available Files

| File | Description | Format | Status |
|------|-------------|--------|--------|
| [**GreenIoT_BOM.csv**](BOM/GreenIoT_BOM.csv) | Complete BOM with pricing and datasheets | CSV | ✅ Production Ready |
| [**GreenIoT_BOM_Simple.csv**](BOM/GreenIoT_BOM_Simple.csv) | Simplified BOM format | CSV | ✅ Production Ready |
| [**COMPONENT_SELECTION_NOTES.md**](BOM/COMPONENT_SELECTION_NOTES.md) | Component selection justification | Markdown | ✅ Complete |
| PCB_BOM.xlsx | Electronic components (legacy format) | Excel | 📋 Legacy |
| Mechanical_BOM.xlsx | Mechanical parts (legacy format) | Excel | 📋 Legacy |

### 💰 Cost Breakdown

**Total Cost per Node**: ₹939,500 IRR (~$18.79 USD)

| Category | Cost (IRR) | Percentage |
|----------|-----------|------------|
| ESP32-C3 Module | ₹250,000 | 26.6% |
| SHT31 Sensor | ₹180,000 | 19.2% |
| 18650 Battery | ₹150,000 | 16.0% |
| PCB & Enclosure | ₹115,000 | 12.2% |
| Power Management ICs | ₹33,000 | 3.5% |
| Passive Components | ₹15,000 | 1.6% |
| Connectors & Switches | ₹15,000 | 1.6% |
| Assembly & Misc | ₹181,500 | 19.3% |
| **Total** | **₹939,500** | **100%** |

### 🔑 Key Components

| Component | Part Number | Quantity | Unit Price | Total |
|-----------|-------------|----------|------------|-------|
| **MCU** | ESP32-C3-DevKitM-1 | 1 | ₹250,000 | ₹250,000 |
| **Sensor** | SHT31-DIS-B | 1 | ₹180,000 | ₹180,000 |
| **LDO** | MCP1700-3302E | 1 | ₹15,000 | ₹15,000 |
| **Charger** | MCP73831T-2ACI | 1 | ₹18,000 | ₹18,000 |
| **Battery** | 18650 Li-Ion 3000mAh | 1 | ₹150,000 | ₹150,000 |

> 💡 **See [Component Selection Notes](BOM/COMPONENT_SELECTION_NOTES.md)** for detailed justification of ultra-low power components

---

## 🔧 Manufacturing Files

### Status: ⏳ In Preparation

Manufacturing files will be organized as follows:

#### Assembly Documentation
- **Assembly_Drawings/**: Visual step-by-step assembly guides
- **Assembly_Guides/**: Detailed written instructions
- **Test_Fixtures/**: Testing jig designs and specifications

#### PCB Manufacturing
- **Raw_PCB/**: Gerber files, drill files, NC files
- **Pick_and_Place/**: Component placement files for SMT
- **Panelization/**: Panel layouts for production efficiency

#### Vendor Information
- **Vendor_Info/**: Supplier contact information and specifications
  - Component suppliers
  - PCB manufacturers
  - Assembly houses
  - Enclosure manufacturers

---

## 📊 Production Planning

### Prototype Phase (Current)
- ✅ Component selection complete
- ✅ BOM created and verified
- ⏳ PCB design in progress
- ⏳ Enclosure design in progress
- 📋 First prototype build planned

### Small Batch Production (Future)
- 📋 10-unit pilot run
- 📋 Production testing protocols
- 📋 Quality assurance procedures
- 📋 Supplier agreements

### Volume Production (Future)
- 📋 100+ unit production
- 📋 Automated assembly
- 📋 Supply chain optimization
- 📋 Cost reduction analysis

---

## ⚙️ Manufacturing Specifications

### PCB Specifications
- **Size**: 50mm × 40mm
- **Layers**: 2-layer FR4
- **Thickness**: 1.6mm
- **Copper Weight**: 1 oz (35μm)
- **Surface Finish**: ENIG (Electroless Nickel Immersion Gold)
- **Solder Mask**: Green
- **Silkscreen**: White

### Assembly Requirements
- **Component Placement**: Both sides
- **SMT Components**: 0603, 0805, 1206, SOT-23, SOT-23-5
- **Through-Hole**: Connectors, battery holder
- **Reflow Profile**: Lead-free (SAC305)
- **Inspection**: AOI (Automated Optical Inspection)

### Testing Requirements
- **Electrical Testing**: ICT (In-Circuit Test)
- **Functional Testing**: Power-on, sensor reading, BLE mesh
- **Environmental**: Temperature cycling, humidity exposure
- **Quality Control**: 100% visual inspection

---

## 📦 Packaging & Shipping

### Packaging Materials
- Anti-static bags for PCB assemblies
- Foam inserts for protection
- Carton boxes (10 units per box)
- Silica gel desiccant packets
- Documentation package

### Labeling
- Product label with QR code
- Serial number tracking
- Regulatory markings (CE, FCC, RoHS)
- Barcode for inventory management

---

## 🔗 Related Documentation

- [📋 Requirements](../01_Requirements/) - System specifications
- [🔌 Hardware Design](../02_Hardware/) - PCB schematics and layouts
- [🧪 Testing](../05_Test_and_Validation/) - Test plans and reports
- [📚 User Documentation](../07_Documentation/) - User manuals and guides
- [📚 Main README](../README.md) - Project overview

---

## 📞 Support

For production-related questions:
- **GitHub Issues**: [Create an issue](https://github.com/hamiikalhor/GreenIot/issues)
- **Component Selection**: See [COMPONENT_SELECTION_NOTES.md](BOM/COMPONENT_SELECTION_NOTES.md)

---

**[⬅️ Back to Main README](../README.md)**

