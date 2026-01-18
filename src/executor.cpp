#include "executor.h"

auto Executor::execute(const DecodedInstruction &instr, arm64::CPUState &cpu)
    -> void {

  switch (instr.type) {
  case InstructionType::ADD_IMM: {
    // logic: rd = rn + imm
    uint64_t val_rn = cpu.getReg(instr.rn);
    uint64_t result = val_rn + instr.imm;
    cpu.setReg(instr.rd, result);
    break;
  }
  case InstructionType::SUB_IMM: {
    // logic: rd = rn - imm
    uint64_t val_rn = cpu.getReg(instr.rn);
    uint64_t result = val_rn - instr.imm;
    cpu.setReg(instr.rd, result);
    break;
  }
  case InstructionType::UNKNOWN:
  default:
    break;
  }
}
