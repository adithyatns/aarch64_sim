#include "decoder.h"

namespace {
// Naming constants makes the bit-masks readable
constexpr uint32_t GROUP_DP_IMM = 0x11;   // 10001
constexpr uint32_t MASK_REG = 0x1F;       // 5 bits
constexpr uint32_t MASK_IMM12 = 0xFFF;    // 12 bits
constexpr uint32_t MASK_SINGLE_BIT = 0x1; // 1 bit
constexpr uint32_t SHIFT_GROUP = 24;
constexpr uint32_t SHIFT_RN = 5;
constexpr uint32_t SHIFT_IMM = 10;
constexpr uint32_t SHIFT_64BIT = 31;
constexpr uint32_t SHIFT_OP = 30;
} // namespace

auto Decoder::decode(uint32_t instr) -> DecodedInstruction {
  DecodedInstruction decoded;

  // Data Processing (Immediate) Group: bits [28:24] == 10001
  uint32_t group = (instr >> SHIFT_GROUP) & MASK_REG;

  if (group == GROUP_DP_IMM) { // 0b10001
    // Extract op bit [30] to distinguish ADD (0) from SUB (1)
    uint32_t operation = (instr >> SHIFT_OP) & MASK_SINGLE_BIT;
    decoded.type =
        (operation == 0) ? InstructionType::ADD_IMM : InstructionType::SUB_IMM;

    decoded.rd = instr & MASK_REG;                   // Bits [4:0]
    decoded.rn = (instr >> SHIFT_RN) & MASK_REG;     // Bits [9:5]
    decoded.imm = (instr >> SHIFT_IMM) & MASK_IMM12; // Bits [21:10]
    decoded.is64Bit =
        ((instr >> SHIFT_64BIT) & MASK_SINGLE_BIT) != 0; // Bit [31]
  }

  return decoded;
}
