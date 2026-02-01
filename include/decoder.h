#pragma once
#include <cstdint>
#include <iostream>

enum class InstructionType { UNKNOWN, ADD_IMM, SUB_IMM, LDR, LDRU64_IMM, STR };

// New: Addressing Modes for Load/Store
enum class AddrMode {
  None,     // For Math (ADD/SUB)
  Offset,   // [Xn, #imm]
  PreIndex, // [Xn, #imm]!
  PostIndex // [Xn], #imm
};

struct DecodedInstruction {
  InstructionType type = InstructionType::UNKNOWN;
  uint8_t rd = 0;
  uint8_t rn = 0;
  int16_t imm = 0;
  AddrMode mode = AddrMode::None;
  bool is64Bit = false;
};

class Decoder {
public:
  static auto decode(uint32_t instr) -> DecodedInstruction;
};
