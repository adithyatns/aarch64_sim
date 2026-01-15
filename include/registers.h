#pragma once
#include <array>
#include <cstdint>

namespace arm64 {
struct CPUState {
  std::array<uint64_t, 31> X; // X0-X30
  uint64_t PC;                // Program Counter
  uint64_t SP;                // Stack Pointer

  // Zero Register logic: returns 0 always
  uint64_t readXZR() const { return 0; }
};
} // namespace arm64
