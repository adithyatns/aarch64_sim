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
/*
| **Code** | **Mnemonic** | **Meaning**                         | **Logic
(PSTATE)**    | | -------- | ------------ | -----------------------------------
| --------------------- | | `0000`   | **EQ**       | Equal | `Z == 1` | |
`0001`   | **NE**       | Not Equal                           | `Z == 0` | |
`0010`   | **CS / HS**  | Carry Set / Unsigned Higher or Same | `C == 1` | |
`0011`   | **CC / LO**  | Carry Clear / Unsigned Lower        | `C == 0` | |
`0100`   | **MI**       | Minus (Negative)                    | `N == 1` | |
`0101`   | **PL**       | Plus (Positive or Zero)             | `N == 0` | |
`0110`   | **VS**       | Overflow Set                        | `V == 1` | |
`0111`   | **VC**       | Overflow Clear                      | `V == 0` | |
`1000`   | **HI**       | Unsigned Higher                     | `C == 1 && Z ==
0`    | | `1001`   | **LS**       | Unsigned Lower or Same              | `C ==
0               | | `1010`   | **GE**       | Signed Greater or Equal | `N == V`
| | `1011`   | **LT**       | Signed Less Than                    | `N != V` |
| `1100`   | **GT**       | Signed Greater Than                 | `Z == 0 && N
== V`    | | `1101`   | **LE**       | Signed Less or Equal                | `Z
== 1               | | `1110`   | **AL**       | Always | `true` (Always jumps)
|
*/
// Helper: Checks if a conditional branch should be taken based on the condition
// code and current PSTATE flags.
auto static check_condition(const arm64::CPUState &cpu, uint8_t cond) -> bool {
  // For simplicity, we only implement a few conditions here
  switch (cond) {
  case 0x0: // EQ (Equal)
    return cpu.pstate.Z;
  case 0x1: // NE (Not Equal)
    return !cpu.pstate.Z;
  case 0x3: // CC/LO (Carry Clear / Unsigned Lower)
    return !cpu.pstate.C;
  case 0x4: // MI (Minus / Negative)
    return cpu.pstate.N;
  case 0x5: // PL (Plus / Positive or Zero)
    return !cpu.pstate.N;
  case 0x6: // VS (Overflow Set)
    return cpu.pstate.V;
  case 0x7: // VC (Overflow Clear)
    return !cpu.pstate.V;
  case 0x8: // HI (Unsigned Higher)
    return cpu.pstate.C && !cpu.pstate.Z;
  case 0xA: // GE (Greater or Equal, signed)
    return cpu.pstate.N == cpu.pstate.V;
  case 0xB: // LT (Less Than, signed)
    return cpu.pstate.N != cpu.pstate.V;

  default:
    std::cerr << "Unsupported condition code: " << static_cast<int>(cond)
              << "\n";
    return false; // Default to not taking the branch
  }
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
  case InstructionType::BRANCH:
    // For simplicity, we won't implement actual branching logic here.
    // In a full implementation, we'd update the PC based on the immediate
    // value.
    std::cout << "Branch instruction encountered. Immediate: " << instr.imm
              << "\n";
    if (instr.imm != 0) {
      cpu.PC += instr.imm; // This is a simplification for demonstration
    }
    break;
  case InstructionType::BRANCH_COND:
    // For simplicity, we won't implement actual conditional branching logic
    // here. In a full implementation, we'd check the condition flags and update
    // the PC.
    if (check_condition(cpu, instr.cond)) { // If condition is met, branch
      cpu.PC += instr.imm; // This is a simplification for demonstration
    } else {
      std::cout << "Conditional Branch with unhandled condition code: "
                << instr.cond << "\n";
    }
    std::cout << "Conditional Branch instruction encountered. Condition: "
              << instr.cond << "\n";
  case InstructionType::UNKNOWN:
  default:
    break;
  }
}
