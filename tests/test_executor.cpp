#include "executor.h"
#include "registers.h"
#include <gtest/gtest.h>

// Fixture Class
class ExecutorTest : public ::testing::Test {
protected:
  // Objects accessible to all tests
  arm64::CPUState cpu;
  Memory memory{1024}; // Initialize 1KB memory

  // Runs before EACH test
  void SetUp() override {
    // Reset CPU State (Registers, SP, PC, Flags)
    cpu.SP = 0;
    cpu.PC = 0;
    for (auto &reg : cpu.X) {
      reg = 0;
    }
    // If PSTATE is implemented, reset it here too
    cpu.pstate = {};

    // Reset Memory (Optional, depending on Memory implementation)
    // memory.reset();
  }

  // Runs after EACH test
  void TearDown() override {
    // Clean up code if necessary
  }
};

// --- Tests ---

TEST_F(ExecutorTest, Execute_ADD_Immediate) {
  // Setup specific to this test
  cpu.setReg(1, 10);

  DecodedInstruction instr;
  instr.type = InstructionType::ADD_IMM;
  instr.rd = 0;
  instr.rn = 1;
  instr.imm = 5;
  instr.is64Bit = true;

  Executor::execute(instr, cpu, memory);

  EXPECT_EQ(cpu.getReg(0), 15);
}

TEST_F(ExecutorTest, Execute_SUB_Immediate) {
  cpu.setReg(2, 20);

  DecodedInstruction instr;
  instr.type = InstructionType::SUB_IMM;
  instr.rd = 3;
  instr.rn = 2;
  instr.imm = 8;
  instr.is64Bit = true;

  Executor::execute(instr, cpu, memory);

  EXPECT_EQ(cpu.getReg(3), 12);
}

TEST_F(ExecutorTest, FullPipelineAdd) {

  cpu.setReg(1, 50);
  uint64_t hexInstr = 0x91006420; // ADD X0, X1, #25
  auto decoded = Decoder::decode(hexInstr);
  Executor::execute(decoded, cpu, memory);
  EXPECT_EQ(cpu.getReg(0), 75);
}

TEST_F(ExecutorTest, Execute_STR_PreIndex_StackPush) {
  // Setup: SP at 1000, Data in X0
  cpu.SP = 1000;
  cpu.setReg(0, 0xDEADBEEF);

  DecodedInstruction instr;
  instr.type = InstructionType::STR;
  instr.rd = 0;  // Source X0
  instr.rn = 31; // Base SP
  instr.imm = -16;
  instr.mode = AddrMode::PreIndex;
  instr.is64Bit = true;

  Executor::execute(instr, cpu, memory);

  // Assert: SP updated, Memory written
  EXPECT_EQ(cpu.SP, 984);
  EXPECT_EQ(memory.read64(984), 0xDEADBEEF);
}

TEST_F(ExecutorTest, Execute_LDR_PostIndex_StackPop) {
  // Setup: SP at 984, Data in Memory
  cpu.SP = 984;
  memory.write64(984, 0xCAFEBABE);

  DecodedInstruction instr;
  instr.type = InstructionType::LDR;
  instr.rd = 1;  // Dest X1
  instr.rn = 31; // Base SP
  instr.imm = 16;
  instr.mode = AddrMode::PostIndex;
  instr.is64Bit = true;

  Executor::execute(instr, cpu, memory);

  // Assert: X1 loaded, SP updated
  EXPECT_EQ(cpu.getReg(1), 0xCAFEBABE);
  EXPECT_EQ(cpu.SP, 1000);
}

TEST_F(ExecutorTest, Execute_STR_Offset_NoUpdate) {
  cpu.setReg(5, 12345); // Data
  cpu.setReg(6, 200);   // Base Address

  DecodedInstruction instr;
  instr.type = InstructionType::STR;
  instr.rd = 5;
  instr.rn = 6;
  instr.imm = 8;
  instr.mode = AddrMode::Offset; // Simple offset
  instr.is64Bit = true;

  Executor::execute(instr, cpu, memory);

  // Assert: Memory written at 208
  EXPECT_EQ(memory.read64(208), 12345);
  // Assert: Base register UNCHANGED
  EXPECT_EQ(cpu.getReg(6), 200);
}

// 1. Test Zero Flag (Z) - CMP X0, X1 where X0 == X1
// Checks that 10 - 10 = 0 sets the Z flag.
TEST_F(ExecutorTest, Execute_CMP_Equality_Sets_Z_Flag) {
  cpu.setReg(0, 10);
  cpu.setReg(1, 10);

  // CMP X0, X1 -> Alias for SUBS XZR, X0, X1
  DecodedInstruction instr;
  instr.type = InstructionType::SUB_REG; // Or SUBS_REG if separated
  instr.rd = 31;                         // XZR (Result discarded)
  instr.rn = 0;                          // X0
  instr.rm = 1;                          // X1
  instr.is64Bit = true;
  instr.setFlags = true; // Essential for CMP behavior

  Executor::execute(instr, cpu, memory);

  // Verify Flags
  EXPECT_EQ(cpu.pstate.Z, 1) << "Z flag should be set (10 - 10 = 0)";
  EXPECT_EQ(cpu.pstate.N, 0);

  // Verify Registers Unchanged (Crucial for CMP)
  EXPECT_EQ(cpu.getReg(0), 10) << "Source X0 should remain 10";
  EXPECT_EQ(cpu.getReg(1), 10) << "Source X1 should remain 10";
}

// 2. Test Negative Flag (N) - CMP X0, X1 where X0 < X1
// Checks that 5 - 10 = -5 sets the N flag.
TEST_F(ExecutorTest, Execute_CMP_Negative_Sets_N_Flag) {
  cpu.setReg(0, 5);
  cpu.setReg(1, 10);

  // CMP X0, X1
  DecodedInstruction instr;
  instr.type = InstructionType::SUB_REG;
  instr.rd = 31; // XZR
  instr.rn = 0;
  instr.rm = 1;
  instr.is64Bit = true;
  instr.setFlags = true;

  Executor::execute(instr, cpu, memory);

  // Assertions
  EXPECT_EQ(cpu.pstate.N, 1) << "N flag should be set (Result is negative)";
  EXPECT_EQ(cpu.pstate.Z, 0);
}

// 3. Test Carry Flag (C) - CMP X0, #Immediate
// Checks unsigned comparison (X0 >= Imm)
TEST_F(ExecutorTest, Execute_CMP_Immediate_Sets_Carry) {
  cpu.setReg(0, 20);

  // CMP X0, #10 -> Alias for SUBS XZR, X0, #10
  DecodedInstruction instr;
  instr.type = InstructionType::SUB_IMM;
  instr.rd = 31; // XZR
  instr.rn = 0;
  instr.imm = 10;
  instr.is64Bit = true;
  instr.setFlags = true;

  Executor::execute(instr, cpu, memory);

  // In ARM, Subtraction Carry = !Borrow.
  // 20 - 10 requires NO borrow, so C=1.
  EXPECT_EQ(cpu.pstate.C, 1) << "C flag should be 1 (No borrow occurred)";
}

// 1. Test B.EQ (Equal) - Taken
// Condition: Z == 1
TEST_F(ExecutorTest, B_EQ_BranchTaken_When_Z_Set) {
  cpu.PC = 0x1000;
  cpu.pstate.Z = 1; // Simulate "Equal" result

  DecodedInstruction instr;
  instr.type = InstructionType::BRANCH_COND;
  instr.cond = 0x0; // EQ (Equal)
  instr.imm = 20;   // Jump forward 20 bytes

  Executor::execute(instr, cpu, memory);

  // Expect PC to jump to 0x1000 + 20
  EXPECT_EQ(cpu.PC, 0x1014);
}

// 2. Test B.EQ (Equal) - Not Taken
// Condition: Z == 0
TEST_F(ExecutorTest, B_EQ_BranchNotTaken_When_Z_Clear) {
  cpu.PC = 0x1000;
  cpu.pstate.Z = 0; // Simulate "Not Equal"

  DecodedInstruction instr;
  instr.type = InstructionType::BRANCH_COND;
  instr.cond = 0x0; // EQ
  instr.imm = 20;

  Executor::execute(instr, cpu, memory);

  // Expect PC to remain unchanged (Simulator loop handles the +4 step)
  EXPECT_EQ(cpu.PC, 0x1000);
}

// 3. Test B.NE (Not Equal) - Taken
// Condition: Z == 0
TEST_F(ExecutorTest, B_NE_BranchTaken_When_Z_Clear) {
  cpu.PC = 0x2000;
  cpu.pstate.Z = 0; // Simulate "Not Equal"

  DecodedInstruction instr;
  instr.type = InstructionType::BRANCH_COND;
  instr.cond = 0x1; // NE (Not Equal)
  instr.imm = -16;  // Jump backward 16 bytes

  Executor::execute(instr, cpu, memory);

  // Expect PC to jump to 0x2000 - 16 = 0x1FF0
  EXPECT_EQ(cpu.PC, 0x1FF0);
}

// 4. Test B.GE (Signed Greater or Equal) - Taken
// Condition: N == V
TEST_F(ExecutorTest, B_GE_BranchTaken_When_N_Equals_V) {
  cpu.PC = 0x3000;

  // Case 1: Positive result (N=0, V=0) -> 0 == 0 -> True
  cpu.pstate.N = 0;
  cpu.pstate.V = 0;

  DecodedInstruction instr;
  instr.type = InstructionType::BRANCH_COND;
  instr.cond = 0xA; // GE (Signed >=)
  instr.imm = 100;

  Executor::execute(instr, cpu, memory);
  EXPECT_EQ(cpu.PC, 0x3064);

  // Reset PC and test Case 2: Negative Overflow (N=1, V=1) -> 1 == 1 -> True
  cpu.PC = 0x3000;
  cpu.pstate.N = 1;
  cpu.pstate.V = 1;
  Executor::execute(instr, cpu, memory);
  EXPECT_EQ(cpu.PC, 0x3064);
}

// 5. Test B.GE (Signed Greater or Equal) - Not Taken
// Condition: N != V
TEST_F(ExecutorTest, B_GE_BranchNotTaken_When_N_NotEqual_V) {
  cpu.PC = 0x3000;

  // Case: Negative result without overflow (N=1, V=0) -> "Less Than"
  cpu.pstate.N = 1;
  cpu.pstate.V = 0;

  DecodedInstruction instr;
  instr.type = InstructionType::BRANCH_COND;
  instr.cond = 0xA; // GE
  instr.imm = 100;

  Executor::execute(instr, cpu, memory);

  // Expect PC to remain unchanged
  EXPECT_EQ(cpu.PC, 0x3000);
}
