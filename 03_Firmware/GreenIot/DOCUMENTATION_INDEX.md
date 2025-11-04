# 📚 Documentation Index

**GreenIoT BLE Mesh Sensor Node - Complete Documentation Map**

---

## 🚀 Quick Start

**New to this project?** Start here:

1. **[README.md](README.md)** ← **START HERE**
   - Project overview
   - Quick start guide
   - Hardware requirements
   - Basic usage

2. **Run Tests** (No Hardware Required)
   ```bash
   ./run_tests.sh mock
   ```
   - ✅ 15/15 tests passing
   - ~2 seconds execution

---

## 📖 Documentation Structure

### Root Level

```
GreenIot/03_Firmware/GreenIot/
│
├── 📘 README.md (811 lines)
│   ├─ Project overview
│   ├─ Architecture explanation
│   ├─ Getting started guide
│   ├─ Sensor abstraction layer
│   └─ Quick testing instructions
│
├── 📋 HARDWARE_TEST_CHECKLIST.md (366 lines)
│   ├─ Hardware procurement checklist
│   ├─ Assembly instructions
│   ├─ Firmware flashing steps
│   ├─ 10 test procedures (printable)
│   └─ Test report template
│
├── 🛠️ run_tests.sh (executable)
│   ├─ Test runner script
│   ├─ Mock tests (./run_tests.sh mock)
│   ├─ Hardware tests (./run_tests.sh hw)
│   └─ Help (./run_tests.sh help)
│
└── 📁 docs/
    ├── 📘 BLE_MESH_IMPLEMENTATION.md (550+ lines)
    └── 📘 TESTING_GUIDE.md (650+ lines)
```

---

## 📘 Detailed Documentation

### 1. README.md
**Purpose:** Main project entry point  
**Audience:** Developers, new users  
**Topics:**
- ✅ Project overview and goals
- ✅ Layered architecture explanation
- ✅ Hardware requirements (ESP32-C3, SHT31)
- ✅ Software structure
- ✅ Getting started guide
- ✅ Sensor abstraction layer
- ✅ Adding new sensors
- ✅ Configuration options
- ✅ API documentation
- ✅ Testing quick start
- ✅ Contributing guidelines

**When to read:** First time setup, understanding project structure

---

### 2. docs/BLE_MESH_IMPLEMENTATION.md
**Purpose:** Technical implementation details  
**Audience:** Embedded developers, system integrators  
**Topics:**
- ✅ BLE Mesh architecture
- ✅ Sensor Server Model implementation
- ✅ Publishing strategy (5-minute interval justification)
- ✅ Low Power Node (LPN) configuration
- ✅ Network configuration (provisioning, addressing)
- ✅ Complete API reference
- ✅ Implementation details (file structure, key functions)
- ✅ Performance metrics
- ✅ Debugging guide

**When to read:** Implementing features, troubleshooting, API reference

---

### 3. docs/TESTING_GUIDE.md
**Purpose:** Complete testing procedures  
**Audience:** QA engineers, developers  
**Topics:**
- ✅ Quick start (run_tests.sh)
- ✅ Unit testing (15 mock tests, hardware tests)
- ✅ Hardware testing (single node)
- ✅ Multi-node testing (3-5 nodes)
  - Provisioning procedures
  - 10 test scenarios
  - Expected outputs
  - Success criteria
- ✅ Power consumption testing
- ✅ Test analysis (automated log analysis)
- ✅ Troubleshooting guide

**When to read:** Running tests, validating hardware, debugging network issues

---

### 4. HARDWARE_TEST_CHECKLIST.md
**Purpose:** Printable hardware validation checklist  
**Audience:** QA engineers, field technicians  
**Topics:**
- ☐ Pre-test preparation (hardware, software)
- ☐ Hardware assembly steps
- ☐ Firmware flashing checklist
- ☐ Network provisioning
- ☐ 10 test procedures with pass/fail boxes
- ☐ Data collection steps
- ☐ Post-test documentation
- ☐ Test summary template

**When to read:** Hardware validation, production testing

---

### 5. test/README.md
**Purpose:** Test directory overview  
**Audience:** Developers  
**Topics:**
- Test file descriptions
- Running tests (quick commands)
- Test results summary
- Links to detailed guides

**When to read:** Understanding test structure

---

## 🛠️ Tools

### run_tests.sh
**Purpose:** Easy test execution  
**Usage:**
```bash
./run_tests.sh mock    # Mock tests (no hardware)
./run_tests.sh hw      # Hardware tests
./run_tests.sh all     # All tests
./run_tests.sh clean   # Clean builds
./run_tests.sh help    # Show help
```

### tools/analyze_mesh_logs.py
**Purpose:** Automated log analysis  
**Usage:**
```bash
python tools/analyze_mesh_logs.py gateway_log.txt
```
**Output:**
- Message delivery statistics
- Network latency analysis
- Hop count distribution
- Sensor data validation
- Plots (mesh_analysis.png)
- CSV export (mesh_data.csv)

---

## 🎯 Use Cases

### "I'm new to this project"
```
1. Read: README.md (overview, quick start)
2. Run: ./run_tests.sh mock
3. Explore: Code structure in src/
```

### "I need to implement a feature"
```
1. Read: README.md (architecture)
2. Read: docs/BLE_MESH_IMPLEMENTATION.md (API reference)
3. Write code following layered architecture
4. Add tests and run ./run_tests.sh
```

### "I need to test hardware"
```
1. Read: docs/TESTING_GUIDE.md (hardware testing)
2. Print: HARDWARE_TEST_CHECKLIST.md
3. Follow checklist step-by-step
4. Analyze: python tools/analyze_mesh_logs.py logs.txt
```

### "I need to provision a mesh network"
```
1. Read: docs/BLE_MESH_IMPLEMENTATION.md (network config)
2. Read: docs/TESTING_GUIDE.md (multi-node section)
3. Use: nRF Mesh app or ESP provisioner
4. Test: Follow Test 1-7 in TESTING_GUIDE.md
```

### "Tests are failing"
```
1. Check: docs/TESTING_GUIDE.md (troubleshooting)
2. Review: Test output logs
3. Debug: Enable verbose logging
4. Analyze: Use log analysis tool
```

---

## 📊 Documentation Statistics

| File | Lines | Purpose |
|------|-------|---------|
| README.md | 811 | Main overview |
| BLE_MESH_IMPLEMENTATION.md | 550+ | Technical details |
| TESTING_GUIDE.md | 650+ | All testing procedures |
| HARDWARE_TEST_CHECKLIST.md | 366 | Printable checklist |
| test/README.md | 80 | Test directory guide |
| **Total** | **~2,450** | **Complete documentation** |

---

## 🗂️ Document Relationships

```
README.md (Start Here)
   │
   ├─→ Quick Start
   │   └─→ ./run_tests.sh mock
   │
   ├─→ Need Technical Details?
   │   └─→ docs/BLE_MESH_IMPLEMENTATION.md
   │
   ├─→ Need to Test?
   │   └─→ docs/TESTING_GUIDE.md
   │
   └─→ Ready for Hardware?
       └─→ HARDWARE_TEST_CHECKLIST.md
```

---

## ✅ Documentation Quality

**Standards Met:**
- ✅ Clear table of contents in each document
- ✅ Consistent formatting (Markdown)
- ✅ Code examples provided
- ✅ Expected outputs shown
- ✅ Troubleshooting sections included
- ✅ Cross-references between documents
- ✅ No duplicate content
- ✅ Up-to-date (Nov 4, 2025)

---

## 🔄 Maintenance

**Last Review:** November 4, 2025  
**Version:** 2.0  
**Status:** ✅ Complete and Current

**Update Frequency:**
- Code changes → Update API docs
- New features → Update README + implementation guide
- Test changes → Update testing guide
- Bug fixes → Update troubleshooting

---

## 📞 Support

**Questions about:**
- Architecture → See README.md
- API → See docs/BLE_MESH_IMPLEMENTATION.md
- Testing → See docs/TESTING_GUIDE.md
- Hardware → See HARDWARE_TEST_CHECKLIST.md

---

**Happy Coding!** 🚀

---

*This index was created to help navigate the consolidated documentation structure.*  
*All duplicate and unnecessary documents have been removed.*  
*Documentation reduced from 10 files to 5 files while preserving all essential information.*

