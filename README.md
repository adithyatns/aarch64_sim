# AArch64 Micro-Simulator

A high-performance ARMv8-A ISA simulator built with C++17.

## Features
- **CPUState:** Full X0-X30 register file with NZCV flags.
- **Modern Toolchain:** CMake-based build system with GTest integration.
- **Static Analysis:** Integrated Clang-Tidy and Clang-Format (Linux Kernel Style).

## Building
```bash
cmake -S . -B build
cmake --build build
```

## Testing
```bash
./build/test_unit_tests
```
