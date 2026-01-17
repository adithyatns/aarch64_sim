#include "decoder.h"
DecodedInstruction Decoder::decode(uint32_t instr) {
  DecodedInstruction decoded;

  // Data Processing (Immediate) Group: bits [28:24] == 10001
  uint32_t group = (instr >> 24) & 0x1F;

  if (group == 0x11) { // 0b10001
    // Extract op bit [30] to distinguish ADD (0) from SUB (1)
    uint32_t op = (instr >> 30) & 0x1;
    decoded.type =
        (op == 0) ? InstructionType::ADD_IMM : InstructionType::SUB_IMM;

    decoded.rd = instr & 0x1F;             // Bits [4:0]
    decoded.rn = (instr >> 5) & 0x1F;      // Bits [9:5]
    decoded.imm = (instr >> 10) & 0xFFF;   // Bits [21:10]
    decoded.is64Bit = (instr >> 31) & 0x1; // Bit [31]
  }

  return decoded;
}
