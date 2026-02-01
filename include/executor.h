#pragma once
#include "decoder.h"
#include "memory.h"
#include "registers.h"

class Executor {
public:
  // Takes the decoded instruction and updates the CPU state accordingly
  static auto execute(const DecodedInstruction &instr, arm64::CPUState &cpu,
                      Memory &mem) -> void;
  static auto read_reg(const arm64::CPUState &cpu, uint8_t reg_idx, bool is_sp)
      -> uint64_t;
  static auto write_reg(arm64::CPUState &cpu, uint8_t reg_idx, uint64_t value,
                        bool is_sp) -> void;
};
