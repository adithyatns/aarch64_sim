#pragma once
#include "decoder.h"
#include "memory.h"
#include "registers.h"

/**
 * @brief Executor class responsible for executing decoded instructions. It
 * takes a DecodedInstruction, updates the CPU state accordingly, and interacts
 * with memory as needed. The execute method will handle the logic for each
 * supported instruction type, including arithmetic operations, memory accesses,
 * and control flow changes. It will also manage the setting of condition flags
 * for CMP instructions and the evaluation of conditions for conditional
 * branches. The read_reg and write_reg helper methods abstract the logic for
 * reading from and writing to registers, including handling the special case of
 * the stack pointer (SP) and the zero register (XZR). This class serves as the
 * core of the instruction execution phase in the simulator, allowing for a
 */
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
