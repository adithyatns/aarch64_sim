#pragma once
#include <cstdint>
#include <iostream>

enum class InstructionType {
  UNKNOWN,
  ADD_IMM,
  SUB_IMM,
  ADD_REG,
  SUB_REG,
  LDR,
  STR,
  BRANCH,
  BRANCH_COND,
};

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
  uint8_t rm = 0; // For SUB_REG
  int16_t imm = 0;
  AddrMode mode = AddrMode::None;
  bool is64Bit = false;
  bool setFlags = 0; // For CMP instructions
  uint8_t cond = 0;  // For conditional branches
};

class Decoder {
public:
  static auto decode(uint32_t instr) -> DecodedInstruction;
};
