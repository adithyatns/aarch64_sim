#include "decoder.h"

namespace {
// Naming constants makes the bit-masks readable
constexpr uint32_t GROUP_DP_IMM = 0b1000;  // 0b1000
constexpr uint32_t GROUP_DP_IMM2 = 0b1001; // 0b1000
constexpr uint32_t GROUP_DP_REG = 0b0101;  // pattern : 0xx101 1101
constexpr uint32_t GROUP_DP_REG_MASK = 0b0111;

constexpr uint32_t GROUP_LS_IMM_MASK = 0b0101; // pattern: x1x0 0b11000

constexpr uint32_t GROUP_BRANCH_IMM = 0b1010;   // pattern 0b101x
constexpr uint32_t GROUP_BRANCH_IMM_2 = 0b1011; // pattern 0b101x

constexpr uint32_t MASK_REG = 0xF; // 4 bits
constexpr uint32_t MASK_REGFILE = 0x1F;
constexpr uint32_t MASK_IMM12 = 0xFFF;    // 12 bits
constexpr uint32_t MASK_SINGLE_BIT = 0x1; // 1 bit
constexpr uint32_t SHIFT_GROUP = 25;
constexpr uint32_t SHIFT_RN = 5;
constexpr uint32_t SHIFT_IMM = 10;
constexpr uint32_t SHIFT_64BIT = 31;
constexpr uint32_t SHIFT_OP = 30;
} // namespace

auto Decoder::decode(uint32_t instr) -> DecodedInstruction {
  DecodedInstruction decoded;
  // Data Processing (Immediate) Group: bits [28:24] == 10001
  uint32_t group = (instr >> SHIFT_GROUP) & MASK_REG;

  if ((group == GROUP_DP_IMM) || (group == GROUP_DP_IMM2)) { // 0b10001
    // Extract op bit [30] to distinguish ADD (0) from SUB (1)
    uint32_t operation = (instr >> SHIFT_OP) & MASK_SINGLE_BIT;
    decoded.type =
        (operation == 0) ? InstructionType::ADD_IMM : InstructionType::SUB_IMM;

    decoded.rd = instr & MASK_REGFILE;                  // Bits [4:0]
    decoded.rn = (instr >> SHIFT_RN) & MASK_REGFILE;    // Bits [9:5]
    uint32_t imm12 = (instr >> SHIFT_IMM) & MASK_IMM12; // Bits [21:10]
    decoded.imm = static_cast<int32_t>(imm12);
    decoded.is64Bit =
        ((instr >> SHIFT_64BIT) & MASK_SINGLE_BIT) != 0; // Bit [31]
    decoded.setFlags = (decoded.rd == 31) ? 1 : 0; // If rd is XZR, set flags
  } else if ((group & GROUP_LS_IMM_MASK) == 0x4) { // 0b11000 or 0b11001
    // Extract op bit [22] to distinguish LDR (1) from STR (0)
    uint32_t operation = (instr >> 22) & MASK_SINGLE_BIT;
    decoded.type =
        (operation == 1) ? InstructionType::LDR : InstructionType::STR;
    decoded.rd = instr & MASK_REGFILE;               // Bits [4:0]
    decoded.rn = (instr >> SHIFT_RN) & MASK_REGFILE; // Bits [9:5]
    bool is_usigned_offset = ((instr >> 24) & MASK_SINGLE_BIT) != 0; // Bit [24]
    if (is_usigned_offset) {
      decoded.mode = AddrMode::Offset;
      uint32_t imm12 = (instr >> SHIFT_IMM) & MASK_IMM12; // Bits [21:10]
      uint8_t size = (instr >> 30) & 0x3;                 // Bits [31:30]
      decoded.imm = static_cast<int32_t>(imm12 << size);
    } else {
      int32_t imm9 = (instr >> 12) & 0x1FF; // Bits [20:12]
      // Sign-extend 9-bit immediate
      if (imm9 & 0x100) {
        imm9 |= ~0x1FF;
      }
      decoded.imm = imm9;
      uint8_t mode_bits = (instr >> 10) & 0x3; // Bits [11:10]
      switch (mode_bits) {
      case 0b01:
        decoded.mode = AddrMode::PostIndex;
        break;
      case 0b11:
        decoded.mode = AddrMode::PreIndex;
        break;
      default:
        decoded.mode = AddrMode::Offset;
        break;
      }
    }
    decoded.is64Bit =
        ((instr >> 30) & 0x3) == 0x3; // Bit [31:30], 64-bit if not 0b11
  } else if ((group >= GROUP_BRANCH_IMM) &&
             (group <= GROUP_BRANCH_IMM_2)) { // 0b1011
    // Future: Handle branch instructions

  } else if ((group & GROUP_DP_REG_MASK) == GROUP_DP_REG) { // 0b0101
    // Extract op bit [30] to distinguish SUB (1)
    uint32_t operation = (instr >> SHIFT_OP) & MASK_SINGLE_BIT;
    decoded.type =
        (operation == 1) ? InstructionType::SUB_REG : InstructionType::ADD_REG;
    decoded.rd = instr & MASK_REGFILE;               // Bits [4:0]
    decoded.rn = (instr >> SHIFT_RN) & MASK_REGFILE; // Bits [9:5]
    decoded.rm = (instr >> 16) & MASK_REGFILE;       // Bits [20:16]
    decoded.is64Bit =
        ((instr >> SHIFT_64BIT) & MASK_SINGLE_BIT) != 0; // Bit [31]
    decoded.setFlags = (decoded.rd == 31) ? 1 : 0; // If rd is XZR, set flags
  } else {
    decoded.type = InstructionType::UNKNOWN;
  }

  return decoded;
}
