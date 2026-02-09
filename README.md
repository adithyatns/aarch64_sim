# AArch64 Simulator

A modular, cycle-accurate-style functional simulator for the ARMv8 AArch64 instruction set architecture (ISA), written in C++.

## 📂 Project Structure

```text
aarch64-sim/
├── src/                # Source implementation (Library: sim_core)
│   ├── decoder.cpp
│   ├── executor.cpp
│   ├── memory.cpp
│   ├── registers.cpp
│   └── CMakeLists.txt  # Defines 'sim_core' library
├── include/            # Header files
│   ├── cpu.h
│   ├── decoder.h
│   ├── executor.h
│   ├── memory.h
│   └── registers.h
├── tests/              # GoogleTest suite
│   ├── test_decoder.cpp
│   ├── test_executor.cpp
│   ├── test_ldr.cpp
│   ├── test_memory.cpp
│   ├── test_registers.cpp
│   └── CMakeLists.txt  # Defines 'unit_tests' executable
├── docs/               # Documentation
│   ├── architecture_hld.md
│   └── system_spec.md
├── CMakeLists.txt      # Root build configuration
└── README.md           # This file
```

## 🚀 Build & Run

### Prerequisites
* **C++ Compiler:** GCC or Clang (supporting C++17)
* **Build System:** CMake (3.14+)
* **Testing:** GoogleTest (automatically fetched via CMake)

### Building the Project
We use CMake to configure and build the system.

```bash
# 1. Create build directory and configure
cmake -S . -B build

# 2. Compile the project
cmake --build build
```

### Running Tests
The test executable is named `unit_tests` and is generated in the `tests/` subdirectory of the build folder.

```bash
# Option 1: Run the test binary directly (Recommended for development)
./build/tests/unit_tests

# Option 2: Run specific test suites using GTest filters
./build/tests/unit_tests --gtest_filter=DecoderTest.*
./build/tests/unit_tests --gtest_filter=ExecutorTest.*

# Option 3: Run using CTest
cd build
ctest --output-on-failure
```

## 🧩 Supported Features

| Feature | Status | Notes |
| :--- | :--- | :--- |
| **Data Processing (Immediate)** | ✅ Done | `ADD`, `SUB`, `SUBS`, `CMP` |
| **Load / Store** | ✅ Done | Offset, Pre-Index, Post-Index modes |
| **Branching** | ✅ Done | Unconditional (`B`) and Conditional (`B.cond`) |
| **Data Processing (Register)** | 🚧 Planned | `ADD` (Reg), `SUB` (Reg) etc. |
| **System Instructions** | 🚧 Planned | `NOP` (Pending) |

## 📚 Documentation
* **High-Level Design:** See `docs/architecture_hld.md` for architectural decisions.
* **System Spec:** See `docs/system_spec.md` for detailed instruction behavior and diagrams.
* **API Reference:** Run `doxygen` to generate HTML documentation in `docs/doxygen/`.
