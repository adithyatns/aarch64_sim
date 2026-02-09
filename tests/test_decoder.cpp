#include "decoder.h"
#include <gtest/gtest.h>

class DecoderTest : public ::testing::Test {
protected:
  // Helper to facilitate decoding
  DecodedInstruction decode(uint32_t instr) { return Decoder::decode(instr); }
};

// --- ADD / SUB (Immediate) ---

TEST_F(DecoderTest, DecodeImmidiate_ADD) {
  uint32_t instrHex = 0x91001420;
  auto result = decode(instrHex);
  EXPECT_EQ(result.type, InstructionType::ADD_IMM);
  EXPECT_EQ(result.rd, 0);
  EXPECT_EQ(result.rn, 1);
  EXPECT_EQ(result.imm, 5);
  EXPECT_TRUE(result.is64Bit);
}

TEST_F(DecoderTest, DecodeImmidiate_SUB) {
  uint32_t instrHex = 0xD1001420;
  auto result = decode(instrHex);
  EXPECT_EQ(result.type, InstructionType::SUB_IMM);
  EXPECT_EQ(result.rd, 0);
  EXPECT_EQ(result.rn, 1);
  EXPECT_EQ(result.imm, 5);
  EXPECT_TRUE(result.is64Bit);
}

TEST_F(DecoderTest, DecodeSubImmediate_SUB2) {
  uint32_t instrHex = 0xD1002862;
  auto result = decode(instrHex);
  EXPECT_EQ(result.type, InstructionType::SUB_IMM);
  EXPECT_EQ(result.rd, 2);
  EXPECT_EQ(result.rn, 3);
  EXPECT_EQ(result.imm, 10);
  EXPECT_TRUE(result.is64Bit);
}

// --- LDR / STR (Load Store) ---

TEST_F(DecoderTest, DecodeImmediate_LDR) {
  // LDR X0, [X1, #8]
  // Hex: 0xF9400420
  uint32_t instrHex = 0xF9400420;
  auto result = decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::LDR);
  EXPECT_EQ(result.rd, 0);  // Target X0
  EXPECT_EQ(result.rn, 1);  // Base X1
  EXPECT_EQ(result.imm, 8); // Offset 8
  EXPECT_EQ(result.mode, AddrMode::Offset);
  EXPECT_TRUE(result.is64Bit);
}

TEST_F(DecoderTest, DecodePreIndex_STR) {
  // STR X30, [SP, #-16]! (Pre-index stack push)
  // Hex: 0xF81F0FFE
  uint32_t instrHex = 0xF81F0FFE;
  auto result = decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::STR);
  EXPECT_EQ(result.rd, 30); // Source X30 (LR)
  EXPECT_EQ(result.rn, 31); // Base SP
  EXPECT_EQ(result.imm, -16);
  EXPECT_EQ(result.mode, AddrMode::PreIndex);
  EXPECT_TRUE(result.is64Bit);
}

TEST_F(DecoderTest, DecodePostIndex_LDR) {
  // LDR X30, [SP], #16 (Post-index stack pop)
  // Hex: 0xF84107FE
  uint32_t instrHex = 0xF84107FE;
  auto result = decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::LDR);
  EXPECT_EQ(result.rd, 30);
  EXPECT_EQ(result.rn, 31);
  EXPECT_EQ(result.imm, 16);
  EXPECT_EQ(result.mode, AddrMode::PostIndex);
  EXPECT_TRUE(result.is64Bit);
}

TEST_F(DecoderTest, DecodeStore_Word) {
  // STR W1, [X2, #4] (32-bit Store)
  // Hex: 0xB9000441
  uint32_t instrHex = 0xB9000441;
  auto result = decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::STR);
  EXPECT_EQ(result.rd, 1);
  EXPECT_EQ(result.rn, 2);
  EXPECT_EQ(result.imm, 4);
  EXPECT_FALSE(result.is64Bit); // 32-bit
}

// --- CMP (Compare / SUBS Alias) ---

TEST_F(DecoderTest, Decode_CMP_Immediate) {
  // Instruction: CMP X0, #42
  // Alias for: SUBS XZR, X0, #42
  // Hex: 0xF100A81F
  uint32_t instr = 0xF100A81F;

  auto decoded = decode(instr);

  EXPECT_EQ(decoded.type, InstructionType::SUB_IMM);
  EXPECT_EQ(decoded.rd, 31); // Destination must be XZR (31)
  EXPECT_EQ(decoded.rn, 0);  // Source is X0
  EXPECT_EQ(decoded.imm, 42);
  EXPECT_TRUE(decoded.is64Bit);
  EXPECT_TRUE(decoded.setFlags);
}

TEST_F(DecoderTest, Decode_CMP_Register) {
  // Instruction: CMP X1, X2
  // Alias for: SUBS XZR, X1, X2
  // Hex: 0xEB02003F
  uint32_t instr = 0xEB02003F;

  auto decoded = decode(instr);

  EXPECT_EQ(decoded.type, InstructionType::SUB_REG);
  EXPECT_EQ(decoded.rd, 31); // XZR
  EXPECT_EQ(decoded.rn, 1);  // X1
  EXPECT_EQ(decoded.rm, 2);  // X2
  EXPECT_TRUE(decoded.is64Bit);
  EXPECT_TRUE(decoded.setFlags);
}

// --- Branch Instructions (New) ---

TEST_F(DecoderTest, Decode_Branch_Unconditional_Forward) {
  // B #16 (Jump forward 4 instructions)
  // Hex: 0x14000004
  auto d = decode(0x14000004);

  EXPECT_EQ(d.type, InstructionType::BRANCH);
  EXPECT_EQ(d.imm, 16);
}

TEST_F(DecoderTest, Decode_Branch_Unconditional_Backward) {
  // B #-16 (Jump backward 4 instructions)
  // Hex: 0x17FFFFFC
  auto d = decode(0x17FFFFFC);

  EXPECT_EQ(d.type, InstructionType::BRANCH);
  EXPECT_EQ(d.imm, -16);
}

TEST_F(DecoderTest, Decode_Branch_Conditional_EQ) {
  // B.EQ #20
  // Hex: 0x540000A0
  auto d = decode(0x540000A0);

  EXPECT_EQ(d.type, InstructionType::BRANCH_COND);
  EXPECT_EQ(d.imm, 20);
  EXPECT_EQ(d.cond, 0x0); // EQ
}

TEST_F(DecoderTest, Decode_Branch_Conditional_NegativeOffset) {
  // B.NE #-8 (Jump back 2 instructions)
  // Hex: 0x54FFFFC1
  auto d = decode(0x54FFFFC1);

  EXPECT_EQ(d.type, InstructionType::BRANCH_COND);
  EXPECT_EQ(d.imm, -8);
  EXPECT_EQ(d.cond, 0x1); // NE
}

// --- Data Processing (Register) ---

TEST_F(DecoderTest, Decode_ADD_Register_64) {
  // ADD X0, X1, X2
  // Binary: 1000 1011 0000 0001 0000 0000 0010 0000
  // Hex: 0x8B020020
  auto d = decode(0x8B020020);
  EXPECT_EQ(d.type, InstructionType::ADD_REG);
  EXPECT_EQ(d.rd, 0);
  EXPECT_EQ(d.rn, 1);
  EXPECT_EQ(d.rm, 2);
  EXPECT_TRUE(d.is64Bit);
}

TEST_F(DecoderTest, Decode_SUB_Register_64) {
  // SUB X5, X6, X7
  // Hex: 0xCB0700C5
  auto d = decode(0xCB0700C5);
  EXPECT_EQ(d.type, InstructionType::SUB_REG);
  EXPECT_EQ(d.rd, 5);
  EXPECT_EQ(d.rn, 6);
  EXPECT_EQ(d.rm, 7);
  EXPECT_TRUE(d.is64Bit);
}

TEST_F(DecoderTest, Decode_SUBS_Register_SetsFlags) {
  // SUBS X0, X1, X2
  // Hex: 0xEB020020
  auto d = decode(0xEB020020);
  EXPECT_EQ(d.type, InstructionType::SUB_REG);
  EXPECT_EQ(d.rd, 0);
  EXPECT_EQ(d.setFlags, true);
}

TEST_F(DecoderTest, Decode_CMP_Register_Alias) {
  // CMP X1, X2 (Alias for SUBS XZR, X1, X2)
  // Hex: 0xEB02003F
  auto d = decode(0xEB02003F);
  EXPECT_EQ(d.type, InstructionType::SUB_REG);
  EXPECT_EQ(d.rd, 31); // XZR
  EXPECT_EQ(d.rn, 1);
  EXPECT_EQ(d.rm, 2);
  EXPECT_EQ(d.setFlags, true);
}
