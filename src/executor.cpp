#include "executor.h"
#include <iostream>

// Helper: Reads a register, handling XZR (31) vs SP (31)
// If is_sp is true: Reg 31 is Stack Pointer.
// If is_sp is false: Reg 31 is Zero Register (Returns 0).
uint64_t Executor::read_reg(const arm64::CPUState &cpu, uint8_t reg_idx,
                            bool is_sp) {
  if (reg_idx == 31) {
    return is_sp ? cpu.SP : 0; // XZR reads as 0
  }
  return cpu.X[reg_idx];
}

// Helper: Writes to a register, handling XZR (31) vs SP (31)
// If is_sp is true: Reg 31 is Stack Pointer.
// If is_sp is false: Reg 31 is Zero Register (Write is ignored).
void Executor::write_reg(arm64::CPUState &cpu, uint8_t reg_idx, uint64_t value,
                         bool is_sp) {
  if (reg_idx == 31) {
    if (is_sp) {
      cpu.SP = value;
    }
    // If XZR, do nothing (write is ignored)
    return;
  }
  cpu.X[reg_idx] = value;
}

auto Executor::execute(const DecodedInstruction &instr, arm64::CPUState &cpu,
                       Memory &mem) -> void {

  switch (instr.type) {
  case InstructionType::ADD_IMM: {
    // logic: rd = rn + imm
    uint64_t val_rn = cpu.getReg(instr.rn);
    uint64_t result = val_rn + instr.imm;
    if (instr.setFlags) {
      // Set Flags
      cpu.pstate.Z = (result == 0);
      cpu.pstate.N = (result >> 63) & 0x1;
      cpu.pstate.C = (result < val_rn); // Check for carry
      // Overflow flag (V) is not typically set for ADD_IMM in CMP
    }
    cpu.setReg(instr.rd, result);
    break;
  }
  case InstructionType::SUB_IMM: {
    // logic: rd = rn - imm
    uint64_t val_rn = cpu.getReg(instr.rn);
    uint64_t result = val_rn - instr.imm;
    if (instr.setFlags) {
      // Set Flags
      cpu.pstate.Z = (result == 0);
      cpu.pstate.N = (result >> 63) & 0x1;
      cpu.pstate.C = (val_rn >= instr.imm); // No borrow
      // Overflow flag (V) is not typically set for SUB_IMM in CMP
    }
    cpu.setReg(instr.rd, result);
    break;
  }
  case InstructionType::ADD_REG: {
    // logic: rd = rn + rm
    uint64_t val_rn = cpu.getReg(instr.rn);
    uint64_t val_rm = cpu.getReg(instr.rm);
    uint64_t result = val_rn + val_rm;
    if (instr.setFlags) {
      // Set Flags
      cpu.pstate.Z = (result == 0);
      cpu.pstate.N = (result >> 63) & 0x1;
      cpu.pstate.C = (result < val_rn); // Check for carry
      // Overflow flag (V) is not typically set for ADD_REG in CMP
    }
    cpu.setReg(instr.rd, result);
    break;
  }
  case InstructionType::SUB_REG: {
    // logic: rd = rn - rm
    uint64_t val_rn = cpu.getReg(instr.rn);
    uint64_t val_rm = cpu.getReg(instr.rm);
    uint64_t result = val_rn - val_rm;
    if (instr.setFlags) {
      // Set Flags
      cpu.pstate.Z = (result == 0);
      cpu.pstate.N = (result >> 63) & 0x1;
      cpu.pstate.C = (val_rn >= val_rm); // No borrow
      // Overflow flag (V) is not typically set for SUB_REG in CMP
    }
    cpu.setReg(instr.rd, result);
    break;
  }
  case InstructionType::LDR: {
    // logic: rd = [rn + imm]
    uint64_t base_addr = (instr.rn == 31) ? cpu.SP : cpu.getReg(instr.rn);
    std::printf(" register: %d, base address %lx, value at base: %lx\n",
                instr.rn, base_addr, mem.read64(base_addr));
    if (instr.mode == AddrMode::PreIndex) {
      base_addr += instr.imm;
      if (instr.rn == 31) {
        cpu.SP = base_addr;
      } else {
        cpu.setReg(instr.rn, base_addr); // Update base register
      }
    } else if (instr.mode == AddrMode::PostIndex) {
      uint64_t temp_addr = base_addr;
      base_addr += instr.imm;
      if (instr.rn == 31) {
        cpu.SP = base_addr;
      } else {
        cpu.setReg(instr.rn, base_addr); // Update base register
      }
      std::cout << "PostIndex Address: " << base_addr << "\n";
      base_addr = temp_addr;
    } else {
      base_addr += instr.imm;
    }
    uint64_t target_addr = base_addr;
    uint64_t result = mem.read64(target_addr);
    cpu.setReg(instr.rd, result);
    break;
  }
  case InstructionType::STR: {
    // logic: [rn + imm] = rd
    uint64_t base_addr = (instr.rn == 31) ? cpu.SP : cpu.getReg(instr.rn);
    uint64_t target_addr = base_addr;
    // Handle addressing modes
    if (instr.mode == AddrMode::Offset) {
      target_addr += instr.imm;
    } else if (instr.mode == AddrMode::PreIndex) {
      target_addr += instr.imm;
      if (instr.rn == 31) {
        cpu.SP = target_addr;
      } else {
        cpu.setReg(instr.rn, target_addr); // Update base register
      }
    } else if (instr.mode == AddrMode::PostIndex) {
      target_addr = base_addr;
      if (instr.rn == 31) {
        cpu.SP = target_addr + instr.imm;
      } else {
        cpu.setReg(instr.rn, target_addr + instr.imm); // Update base register
      }
    }
    uint64_t val_rd = (instr.rd == 31) ? cpu.SP : cpu.getReg(instr.rd);
    mem.write64(target_addr, val_rd);
    break;
  }
  case InstructionType::UNKNOWN:
  default:
    break;
  }
}
