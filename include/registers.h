#pragma once
#include <array>
#include <cstdint>

namespace arm64 {

/**
 * @brief CPUState struct to represent the state of the CPU, including
 * general-purpose registers (X0-X30), the program counter (PC), the stack
 * pointer (SP), and the condition flags (N, Z, C, V). The struct provides
 * methods to read and write registers, with special handling for the zero
 * register (XZR) which always returns 0 when read. This struct serves as the
 * central representation of the CPU's state during instruction execution in the
 * simulator. Note: The getReg and setReg methods will handle the logic for
 * accessing registers, including the special case for XZR. The PC and SP are
 * treated as separate fields for clarity, but they can also be accessed through
 * the getReg and setReg methods if desired. The condition flags are stored in a
 * nested struct for better organization.
 *
 */
constexpr uint8_t REG_XZR = 31;
/**
 * @brief CPUState struct to represent the state of the CPU, including
 * general-purpose registers (X0-X30), the program counter (PC), the stack
 * pointer (SP), and the condition flags (N, Z, C, V). The struct provides
 * methods to read and write registers, with special handling for the zero
 * register (XZR) which always returns 0 when read. This struct serves as the
 * central representation of the CPU's state during instruction execution in the
 * simulator. Note: The getReg and setReg methods will handle the logic for
 * accessing registers, including the special case for XZR. The PC and SP are
 * treated as separate fields for clarity, but they can also be accessed through
 * the getReg and setReg methods if desired. The condition flags are stored in a
 * nested struct for better organization.
 *
 */
struct CPUState {
  std::array<uint64_t, REG_XZR> X; // X0-X30
  uint64_t PC;                     // Program Counter
  uint64_t SP;                     // Stack Pointer
  struct {
    bool N; // Negative Flag
    bool Z; // Zero Flag
    bool C; // Carry Flag
    bool V; // Overflow Flag
  } pstate;
  // Core Register Logic
  auto getReg(uint8_t regId) const -> uint64_t;
  auto setReg(uint8_t regId, uint64_t value) -> void;

  // Zero Register logic: returns 0 always
  auto readXZR() const -> uint64_t { return 0; }
};
} // namespace arm64
