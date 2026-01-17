#pragma once
#include <cstdint>

enum class InstructionType { UNKNOWN, ADD_IMM, SUB_IMM };

struct DecodedInstruction {
  InstructionType type = InstructionType::UNKNOWN;
  uint8_t rd = 0;
  uint8_t rn = 0;
  uint16_t imm = 0;
  bool is64Bit = false;
};

class Decoder {
public:
  static auto decode(uint32_t instr) -> DecodedInstruction;
};
