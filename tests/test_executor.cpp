#include "executor.h"
#include "registers.h"
#include <gtest/gtest.h>

TEST(ExecutorTest, ExecuteAddImmediate) {
  arm64::CPUState cpu;
  Memory mem(1024);
  cpu.setReg(1, 10);
  DecodedInstruction instr;
  instr.type = InstructionType::ADD_IMM;
  instr.rd = 0;
  instr.rn = 1;
  instr.imm = 5;
  Executor::execute(instr, cpu, mem);
  EXPECT_EQ(cpu.getReg(0), 15);
}

TEST(ExecutorTest, ExecuteSubImmediate) {
  arm64::CPUState cpu;
  Memory mem(1024);
  cpu.setReg(2, 20);
  DecodedInstruction instr;
  instr.type = InstructionType::SUB_IMM;
  instr.rd = 3;
  instr.rn = 2;
  instr.imm = 8;
  Executor::execute(instr, cpu, mem);
  EXPECT_EQ(cpu.getReg(3), 12);
}

TEST(ExecutorTest, FullPipelineAdd) {
  arm64::CPUState cpu;
  cpu.setReg(1, 50);
  Memory mem(1024);
  uint64_t hexInstr = 0x91006420; // ADD X0, X1, #25
  auto decoded = Decoder::decode(hexInstr);
  Executor::execute(decoded, cpu, mem);
  EXPECT_EQ(cpu.getReg(0), 75);
}

TEST(ExecutorTest, ExecuteStr_PreIndex_Push) {
  // Goal: Simulate STR X0, [SP, #-16]!
  arm64::CPUState cpu;
  Memory mem(4096);

  // Setup: SP (Reg 31) at 1000, X0 has data
  cpu.SP = 1000;
  cpu.setReg(0, 0xDEADBEEF);

  DecodedInstruction instr;
  instr.type = InstructionType::STR;
  instr.rd = 0;  // Source X0
  instr.rn = 31; // Base SP
  instr.imm = -16;
  instr.mode = AddrMode::PreIndex; // Update SP before write
  instr.is64Bit = true;

  Executor::execute(instr, cpu, mem);

  // Assert 1: SP should be decremented (1000 - 16 = 984)
  EXPECT_EQ(cpu.SP, 984);

  // Assert 2: Memory at 984 should hold 0xDEADBEEF
  // Note: Assuming Memory class has read64/readDoubleWord
  EXPECT_EQ(mem.read64(984), 0xDEADBEEF);
}

TEST(ExecutorTest, ExecuteLdr_PostIndex_Pop) {
  // Goal: Simulate LDR X0, [SP], #16
  arm64::CPUState cpu;
  Memory mem(4096);

  // Setup: SP at 984, Memory at 984 has data
  cpu.SP = 984;
  mem.write64(984, 0xCAFEBABE);

  DecodedInstruction instr;
  instr.type = InstructionType::LDR;
  instr.rd = 0;  // Dest X0
  instr.rn = 31; // Base SP
  instr.imm = 16;
  instr.mode = AddrMode::PostIndex; // Update SP after read
  instr.is64Bit = true;

  Executor::execute(instr, cpu, mem);

  // Assert 1: X0 should have loaded value
  EXPECT_EQ(cpu.getReg(0), 0xCAFEBABE);

  // Assert 2: SP should be incremented (984 + 16 = 1000)
  EXPECT_EQ(cpu.SP, 1000);
}

TEST(ExecutorTest, ExecuteStr_Offset) {
  // Goal: Simulate STR X0, [X1, #8] (No writeback)
  arm64::CPUState cpu;
  Memory mem(1024);

  cpu.setReg(0, 555); // Data
  cpu.setReg(1, 100); // Base Address

  DecodedInstruction instr;
  instr.type = InstructionType::STR;
  instr.rd = 0;
  instr.rn = 1;
  instr.imm = 8;
  instr.mode = AddrMode::Offset;
  instr.is64Bit = true;

  Executor::execute(instr, cpu, mem);

  // Assert 1: Memory at 108 (100 + 8) matches data
  EXPECT_EQ(mem.read64(108), 555);

  // Assert 2: Base Register X1 should NOT change
  EXPECT_EQ(cpu.getReg(1), 100);
}
