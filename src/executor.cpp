#include "executor.h"
#include <iostream>
auto Executor::execute(const DecodedInstruction &instr, arm64::CPUState &cpu,
                       Memory &mem) -> void {

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
  case InstructionType::LDR: {
    // logic: rd = [rn + imm]
    uint64_t base_addr = cpu.X[instr.rn];
    uint64_t target_addr = base_addr + instr.imm;
    uint64_t result = mem.read64(target_addr);
    cpu.setReg(instr.rd, result);
    break;
  }
  case InstructionType::UNKNOWN:
  default:
    break;
  }
}
