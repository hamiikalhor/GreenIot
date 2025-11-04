# 📚 GreenIoT Documentation Index

**Complete Guide to All Project Documentation**

Last Updated: November 4, 2025

---

## 🗺️ Quick Navigation

<div align="center">

| [📋 Requirements](#-requirements--specifications) | [🔌 Hardware](#-hardware-design) | [💻 Firmware](#-firmware) | [🧪 Testing](#-testing--validation) |
|---|---|---|---|
| [🏭 Production](#-production-files) | [📚 User Docs](#-user-documentation) | [📊 Management](#-project-management) | [🤝 Contributing](#-contributing) |

</div>

---

## 📋 Requirements & Specifications

**Location**: [`01_Requirements/`](01_Requirements/)  
**Status**: ✅ Complete (100%)

### Available Documents

| Document | Format | Description |
|----------|--------|-------------|
| [README](01_Requirements/README.md) | Markdown | Requirements overview and summary |
| Product_Specifications.docx | Word | Complete product specification |
| Electrical_Requirements.xlsx | Excel | Electrical design specifications |
| Mechanical_Requirements.xlsx | Excel | Mechanical constraints |
| Safety_and_Compliance_Standards.docx | Word | Regulatory compliance |
| Functional_Block_Diagram.vsdx | Visio | System architecture diagrams |

### Key Specifications

- **MCU**: ESP32-C3 (RISC-V @ 160MHz)
- **Sensor**: SHT31-DIS-B (±0.3°C, ±2% RH)
- **Power**: 18650 Li-Ion 3.7V 3000mAh
- **Battery Life**: 5-7 years
- **Network**: BLE Mesh 5.0

---

## 🔌 Hardware Design

**Location**: [`02_Hardware/`](02_Hardware/)  
**Status**: ⏳ Validation Pending (80%)

### Available Documents

| Document | Format | Description |
|----------|--------|-------------|
| [README](02_Hardware/README.md) | Markdown | Hardware design overview |
| Altium/ | Design Files | PCB schematics and layouts |
| Simulation/ | Analysis | SPICE models and power analysis |
| Hardware_Review_Reports/ | Reports | Design review documentation |
| Sent_For_Print/ | Manufacturing | Gerber files for production |

### Design Highlights

- **PCB**: 50mm × 40mm, 2-layer FR4
- **Ultra-Low Power**: MCP1700 LDO (1.6μA quiescent)
- **Battery Charger**: MCP73831 (500mA, SOT-23-5)
- **I2C**: GPIO8 (SDA), GPIO9 (SCL)
- **Sleep Current**: ~12μA system total

---

## 💻 Firmware

**Location**: [`03_Firmware/GreenIot/`](03_Firmware/GreenIot/)  
**Status**: ✅ Production Ready (100%)

### Documentation

| Document | Description |
|----------|-------------|
| Source Code | C++17 layered architecture |
| Platform | PlatformIO + Arduino/ESP-IDF |
| Architecture | Core → Application → Services → HAL → Drivers |
| Tests | 28/28 passing, 92% coverage |

### Architecture Layers

1. **Core**: main.cpp, Arduino setup()/loop()
2. **Application**: StateMachine (INIT → IDLE → MEASURE → TRANSMIT → SLEEP)
3. **Services**: PowerManager, BLE Mesh Manager
4. **HAL**: ISensor interface, SHT31Sensor implementation
5. **Drivers**: I2C, ADC, GPIO drivers

### Key Features

- ✅ Deep sleep power management
- ✅ BLE Mesh 5.0 networking
- ✅ Hardware sensor abstraction
- ✅ Battery monitoring
- ✅ Basil cultivation optimization

---

## 🧪 Testing & Validation

**Location**: [`05_Test_and_Validation/`](05_Test_and_Validation/)  
**Status**: ⏳ In Progress (75%)

### Available Documents

| Document | Format | Description |
|----------|--------|-------------|
| [README](05_Test_and_Validation/README.md) | Markdown | Testing overview |
| Test_Plans/ | Plans | Test strategy and planning |
| Test_Procedures/ | Procedures | Detailed test procedures |
| Test_Reports/ | Reports | Test results |
| Firmware_Test_Logs/ | Logs | Unit test results |
| EMC_Results/ | Reports | EMC/EMI testing |
| Thermal_Test/ | Data | Environmental testing |

### Test Results

- **Unit Tests**: 10/10 passing (95% coverage)
- **Integration Tests**: 18/18 passing (90% coverage)
- **Total**: 28/28 passing (92% overall)
- **Quality**: 0 warnings, 100% reviewed

---

## 🏭 Production Files

**Location**: [`06_Production/`](06_Production/)  
**Status**: ⏳ Preparation Phase (40%)

### Available Documents

| Document | Format | Description |
|----------|--------|-------------|
| [README](06_Production/README.md) | Markdown | Production overview |
| [GreenIoT_BOM.csv](06_Production/BOM/GreenIoT_BOM.csv) | CSV | Complete BOM with pricing |
| [GreenIoT_BOM_Simple.csv](06_Production/BOM/GreenIoT_BOM_Simple.csv) | CSV | Simplified BOM format |
| [Component Selection Notes](06_Production/BOM/COMPONENT_SELECTION_NOTES.md) | Markdown | Component justification |
| PCB_BOM.xlsx | Excel | Electronic components (legacy) |
| Mechanical_BOM.xlsx | Excel | Mechanical parts (legacy) |

### Cost Summary

- **Total per Node**: ₹939,500 IRR (~$18.79 USD)
- **Key Components**: ESP32-C3 (₹250k), SHT31 (₹180k), Battery (₹150k)
- **Power ICs**: MCP1700 (₹15k), MCP73831 (₹18k)

### Manufacturing Files

- Assembly_Drawings/
- Assembly_Guides/
- Pick_and_Place/
- Raw_PCB/ (Gerber files)
- Panelization/
- Test_Fixtures/
- Vendor_Info/

---

## 📚 User Documentation

**Location**: [`07_Documentation/`](07_Documentation/)  
**Status**: ⏳ Being Prepared (30%)

### Available Documents

| Document | Format | Status |
|----------|--------|--------|
| [README](07_Documentation/README.md) | Markdown | ✅ Complete |
| User_Manual/ | Guide | 📋 Planned |
| Installation_Guide/ | Guide | 📋 Planned |
| Maintenance_Guide/ | Manual | 📋 Planned |
| Datasheets/ | PDFs | 📋 Collection |
| Marketing_Materials/ | Various | 📋 Planned |

### Quick Reference

- [ESP32-C3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)
- [SHT31 Datasheet](https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf)
- [MCP1700 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/20001826E.pdf)
- [MCP73831 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP73831-Family-Data-Sheet-DS20001984H.pdf)
- [BLE Mesh Spec](https://www.bluetooth.com/specifications/specs/mesh-profile-1-0-1/)

---

## 📊 Project Management

**Location**: [`08_Project_Management/`](08_Project_Management/)  
**Status**: ✅ Active (100%)

### Available Documents

| Document | Format | Description |
|----------|--------|-------------|
| [README](08_Project_Management/README.md) | Markdown | Project overview |
| Gantt_Chart/ | Timeline | Project schedule |
| Budget/ | Financial | Cost tracking |
| Meeting_Minutes/ | Notes | Decision logs |
| Task_Tracker/ | Issues | GitHub issues |
| Contracts_and_NDA/ | Legal | Agreements |

### Project Status

- **Milestones**: 4/4 completed (100%)
- **Issues**: 5/5 closed (100%)
- **Budget**: Under by 12.7%
- **Schedule**: On time ✅
- **Quality**: 98% overall score

### Milestones

1. ✅ **v0.1** — Sensor PoC (1 issue)
2. ✅ **v0.2** — BLE Mesh Communication (2 issues)
3. ✅ **v0.3** — Power Optimization (1 issue)
4. ✅ **v1.0** — Final PoC & Documentation (1 issue)

---

## 🤝 Contributing

**Location**: [README.md - Contributing Section](README.md#-contributing)  
**Status**: ✅ Complete

### Git Workflow

We follow an **issue-driven development** workflow:

1. Create/Select Issue → Label → Assign to Milestone
2. Create Branch (`{issue-number}-description`)
3. Make Changes → Test → Commit
4. Push → Create Pull Request
5. Review → Merge → Close Issue

### Branch Naming

```bash
# Format: {issue-number}-{short-description}
git checkout -b 7-add-battery-monitoring
```

### Commit Convention

```bash
# Format: <type>(<scope>): <subject>
git commit -m "feat(sensor): add SHT40 sensor support"
```

### Types

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation
- `style`: Code style
- `refactor`: Code refactoring
- `perf`: Performance
- `test`: Testing
- `build`: Build system
- `ci`: CI/CD
- `chore`: Maintenance

---

## 📖 Main README

**Location**: [README.md](README.md)  
**Status**: ✅ Complete (100%)

### Sections

1. **Overview** - Project introduction and key features
2. **Project Map** - Visual repository structure
3. **Repository Structure** - Detailed directory tree
4. **Architecture** - System and firmware architecture
5. **Quick Start** - 5-minute setup guide
6. **Documentation Navigation** - Complete documentation map ⭐
7. **Hardware** - Component specifications
8. **Software** - Technology stack
9. **Testing** - Test results
10. **Project Status** - Current status and metrics
11. **Contributing** - Git workflow and guidelines ⭐
12. **Support & Resources** - Links and references
13. **License** - MIT License
14. **Team & Acknowledgments** - Credits

---

## 🔍 Finding Information

### By Topic

| Topic | Location |
|-------|----------|
| **Pin Configuration** | [Requirements](01_Requirements/README.md#pin-configuration-esp32-c3) \| [Hardware](02_Hardware/README.md#pin-configuration) |
| **Power Consumption** | [Requirements](01_Requirements/README.md#power-specifications) \| [Component Notes](06_Production/BOM/COMPONENT_SELECTION_NOTES.md) |
| **BLE Mesh Setup** | [Firmware](03_Firmware/GreenIot/) \| [User Docs](07_Documentation/README.md) |
| **Component Selection** | [Component Notes](06_Production/BOM/COMPONENT_SELECTION_NOTES.md) ⭐ |
| **Test Procedures** | [Testing](05_Test_and_Validation/README.md) |
| **Cost Breakdown** | [Production](06_Production/README.md#cost-breakdown) |
| **Git Workflow** | [Contributing](README.md#-git-workflow) ⭐ |
| **Milestones** | [Project Management](08_Project_Management/README.md#milestone-timeline) |

### By Role

| Role | Start Here |
|------|------------|
| **👨‍💻 Developer** | [Firmware](03_Firmware/GreenIot/) → [Contributing](README.md#-contributing) |
| **🔧 Hardware Engineer** | [Hardware](02_Hardware/README.md) → [BOM](06_Production/BOM/) |
| **🧪 QA Engineer** | [Testing](05_Test_and_Validation/README.md) |
| **📋 Project Manager** | [Project Management](08_Project_Management/README.md) |
| **👤 End User** | [User Documentation](07_Documentation/README.md) |

---

## 📞 Support

### Questions or Issues?

- **GitHub Issues**: [Create an issue](https://github.com/hamiikalhor/GreenIot/issues)
- **Discussions**: [GitHub Discussions](https://github.com/hamiikalhor/GreenIot/discussions)
- **Email**: Contact the GreenIoT Development Team

### Documentation Feedback

Found an error or have a suggestion for documentation improvement?

1. Open an issue with label `documentation`
2. Provide specific page/section reference
3. Suggest improvement or correction

---

## 🔄 Documentation Updates

This documentation index is maintained alongside the project. Last major updates:

| Date | Update |
|------|--------|
| Nov 4, 2025 | Created comprehensive documentation structure |
| Nov 4, 2025 | Added Component Selection Notes |
| Nov 4, 2025 | Integrated Git Workflow documentation |
| Nov 4, 2025 | Created README files for all major directories |
| Nov 4, 2025 | Added BOM CSV files and integration |

---

**[⬅️ Back to Main README](README.md)**

