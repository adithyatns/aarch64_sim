#include "executor.h"
#include "instruction.h"
#include "memory.h"
#include "registers.h"
#include <gtest/gtest.h>

// Test Fixture (Optional, but keeps code clean)
class LDRTest : public ::testing::Test {
protected:
  arm64::CPUState state;
  Memory ram;

  LDRTest() : ram(4096) { // Initialize 4KB RAM
    // Clear Registers before every test
    for (int i = 0; i < 32; i++)
      state.X[i] = 0;
    state.PC = 0;
    state.SP = 0;
  }
};

TEST_F(LDRTest, LDR_Loads_64Bit_Value_From_Memory) {
  // 1. Setup
  // Memory ram(1024);
  // arm64::CPUState state;
  //  2. data preparation
  uint64_t target_addr = 0x100;
  uint64_t expectedValue = 0x1122334455667788;
  ram.write64(target_addr, expectedValue);

  // 3. Set Base Register X1 to point to address 0x10
  state.setReg(1, target_addr);

  // 4. Execute LDR X0, [X1]
  //  Opcode: 0xF9400020
  DecodedInstruction instr;
  instr.type = InstructionType::LDR;
  instr.rd = 0;
  instr.rn = 1;
  instr.imm = 0;

  // 5. Assert
  Executor::execute(instr, state, ram);
  // This will FAIL initially because execute() doesn't handle LDR yet
  EXPECT_EQ(state.getReg(0), 0x1122334455667788);
}

TEST_F(LDRTest, Load_With_Positive_Immediate_Offset) {
  // Scenario: Accessing a field in a struct or stack var
  // LDR X2, [X1, #8] -> Load from (X1 + 8)

  // 1. Setup Base Address
  uint64_t baseAddr = 0x200;
  state.X[1] = baseAddr;

  // 2. Write data at Base + 8 (0x208)
  uint64_t expectedValue = 0xAAAA5555AAAA5555;
  ram.write64(baseAddr + 8, expectedValue);

  // 3. Construct Instruction: LDR X2, [X1, #8]
  DecodedInstruction instr;
  instr.type = InstructionType::LDR;
  instr.rd = 2;  // Target: X2
  instr.rn = 1;  // Base: X1
  instr.imm = 8; // Offset: +8

  // 4. Execute
  Executor::execute(instr, state, ram);

  // 5. Verify
  EXPECT_EQ(state.X[2], expectedValue);
}

TEST_F(LDRTest, Load_With_Negative_Immediate_Offset) {
  // Scenario: Accessing previous stack frame or array index
  // LDR X3, [X1, #-16] -> Load from (X1 - 16)

  // 1. Setup Base Address
  uint64_t baseAddr = 0x300;
  state.X[1] = baseAddr;

  // 2. Write data at Base - 16 (0x2F0)
  uint64_t expectedValue = 0x123456789ABCDEF0;
  ram.write64(baseAddr - 16, expectedValue);

  // 3. Construct Instruction: LDR X3, [X1, #-16]
  DecodedInstruction instr;
  instr.type = InstructionType::LDR;
  instr.rd = 3;    // Target: X3
  instr.rn = 1;    // Base: X1
  instr.imm = -16; // Offset: -16

  // 4. Execute
  Executor::execute(instr, state, ram);

  // 5. Verify
  EXPECT_EQ(state.X[3], expectedValue);
}

TEST_F(LDRTest, Load_From_Unmapped_Memory_Returns_Zero) {
  // Safety Check: Reading outside bounds should not crash
  // Reading from address 0x5000 (Beyond 4KB size)

  state.X[1] = 0x5000;

  DecodedInstruction instr;
  instr.type = InstructionType::LDR;
  instr.rd = 4;
  instr.rn = 1;
  instr.imm = 0;

  Executor::execute(instr, state, ram);

  EXPECT_EQ(state.X[4], 0); // Assuming Memory returns 0 on OOB
}
