#pragma once
#include <array>
#include <cstdint>

namespace arm64 {

// Define the constant for the Zero Register Index
constexpr uint8_t REG_XZR = 31;

struct CPUState {
  std::array<uint64_t, REG_XZR> X; // X0-X30
  uint64_t PC;                     // Program Counter
  uint64_t SP;                     // Stack Pointer

  // Core Register Logic
  auto getReg(uint8_t regId) const -> uint64_t;
  auto setReg(uint8_t regId, uint64_t value) -> void;

  // Zero Register logic: returns 0 always
  auto readXZR() const -> uint64_t { return 0; }
};
} // namespace arm64
