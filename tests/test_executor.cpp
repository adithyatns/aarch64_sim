#include "executor.h"
#include "registers.h"
#include <gtest/gtest.h>

TEST(ExecutorTest, ExecuteAddImmediate) {
  arm64::CPUState cpu;

  // Setup: X1 = 10
  cpu.setReg(1, 10);

  // Simulate a decoded instruction: ADD X0, X1, #5
  DecodedInstruction instr;
  instr.type = InstructionType::ADD_IMM;
  instr.rd = 0;  // Target: X0
  instr.rn = 1;  // Source: X1
  instr.imm = 5; // Add 5

  // Act
  Executor::execute(instr, cpu);

  // Assert: X0 should be 10 + 5 = 15
  EXPECT_EQ(cpu.getReg(0), 15);
}

TEST(ExecutorTest, ExecuteSubImmediate) {
  arm64::CPUState cpu;
  cpu.setReg(2, 20); // X2 = 20

  // Simulate: SUB X3, X2, #8
  DecodedInstruction instr;
  instr.type = InstructionType::SUB_IMM;
  instr.rd = 3;
  instr.rn = 2;
  instr.imm = 8;

  Executor::execute(instr, cpu);

  EXPECT_EQ(cpu.getReg(3), 12); // 20 - 8 = 12
}

TEST(ExecutorTest, FullPipelineAdd) {
  arm64::CPUState cpu;
  cpu.setReg(1, 50);

  // Hex for: ADD X0, X1, #25
  uint64_t hexInstr = 0x91006420;

  // step 1 : decode
  auto decoded = Decoder::decode(hexInstr);

  // step 2 : execute
  Executor::execute(decoded, cpu);

  // Assert
  EXPECT_EQ(cpu.getReg(0), 75);
}
