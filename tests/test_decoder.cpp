#include "decoder.h"
#include <gtest/gtest.h>

// --- Existing Tests ---
TEST(DecoderTest, DecodeImmidiate_ADD) {
  uint32_t instrHex = 0x91001420;
  auto result = Decoder::decode(instrHex);
  EXPECT_EQ(result.type, InstructionType::ADD_IMM);
  EXPECT_EQ(result.rd, 0);
  EXPECT_EQ(result.rn, 1);
  EXPECT_EQ(result.imm, 5);
  EXPECT_TRUE(result.is64Bit);
}

TEST(DecoderTest, DecodeImmidiate_SUB) {
  uint32_t instrHex = 0xD1001420;
  auto result = Decoder::decode(instrHex);
  EXPECT_EQ(result.type, InstructionType::SUB_IMM);
  EXPECT_EQ(result.rd, 0);
  EXPECT_EQ(result.rn, 1);
  EXPECT_EQ(result.imm, 5);
  EXPECT_TRUE(result.is64Bit);
}

TEST(DecoderTest, DecodeSubImmediate_SUB2) {
  uint32_t instrHex = 0xD1002862;
  auto result = Decoder::decode(instrHex);
  EXPECT_EQ(result.type, InstructionType::SUB_IMM);
  EXPECT_EQ(result.rd, 2);
  EXPECT_EQ(result.rn, 3);
  EXPECT_EQ(result.imm, 10);
  EXPECT_TRUE(result.is64Bit);
}

// --- NEW Tests for LDR / STR ---

TEST(DecoderTest, DecodeImmediate_LDR) {
  // LDR X0, [X1, #8]
  // Hex: 0xF9400420
  uint32_t instrHex = 0xF9400420;
  auto result = Decoder::decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::LDR);
  EXPECT_EQ(result.rd, 0);  // Target X0
  EXPECT_EQ(result.rn, 1);  // Base X1
  EXPECT_EQ(result.imm, 8); // Offset 8
  EXPECT_EQ(result.mode, AddrMode::Offset);
  EXPECT_TRUE(result.is64Bit);
}

TEST(DecoderTest, DecodePreIndex_STR) {
  // STR X30, [SP, #-16]! (Pre-index stack push)
  // Hex: 0xF81F0FFE
  uint32_t instrHex = 0xF81F0FFE;
  auto result = Decoder::decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::STR);
  EXPECT_EQ(result.rd, 30); // Source X30 (LR)
  EXPECT_EQ(result.rn, 31); // Base SP
  EXPECT_EQ(result.imm, -16);
  EXPECT_EQ(result.mode, AddrMode::PreIndex);
  EXPECT_TRUE(result.is64Bit);
}

TEST(DecoderTest, DecodePostIndex_LDR) {
  // LDR X30, [SP], #16 (Post-index stack pop)
  // Hex: 0xF84107FE
  uint32_t instrHex = 0xF84107FE;
  auto result = Decoder::decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::LDR);
  EXPECT_EQ(result.rd, 30);
  EXPECT_EQ(result.rn, 31);
  EXPECT_EQ(result.imm, 16);
  EXPECT_EQ(result.mode, AddrMode::PostIndex);
  EXPECT_TRUE(result.is64Bit);
}

TEST(DecoderTest, DecodeStore_Word) {
  // STR W1, [X2, #4] (32-bit Store)
  // Hex: 0xB9000441
  uint32_t instrHex = 0xB9000441;
  auto result = Decoder::decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::STR);
  EXPECT_EQ(result.rd, 1);
  EXPECT_EQ(result.rn, 2);
  EXPECT_EQ(result.imm, 4);
  EXPECT_FALSE(result.is64Bit); // 32-bit
}

TEST(DecoderTest, Decode_CMP_Immediate) {
  // Instruction: CMP X0, #42
  // Alias for: SUBS XZR, X0, #42
  // Hex: 0xF100A81F
  // Binary: 1111 0001 0000 0000 1010 1000 0001 1111
  uint32_t instr = 0xF100A81F;

  auto decoded = Decoder::decode(instr);

  // CMP is just SUBS with specific operands
  EXPECT_EQ(decoded.type, InstructionType::SUB_IMM);
  EXPECT_EQ(decoded.rd, 31); // Destination must be XZR (31)
  EXPECT_EQ(decoded.rn, 0);  // Source is X0
  EXPECT_EQ(decoded.imm, 42);
  EXPECT_TRUE(decoded.is64Bit);
  // You may need to add a 'setFlags' field to your struct later
  EXPECT_TRUE(decoded.setFlags);
}

TEST(DecoderTest, Decode_CMP_Register) {
  // Instruction: CMP X1, X2
  // Alias for: SUBS XZR, X1, X2
  // Hex: 0xEB02003F
  uint32_t instr = 0xEB02003F;

  auto decoded = Decoder::decode(instr);

  EXPECT_EQ(decoded.type, InstructionType::SUB_REG); // Or generic SUBS
  EXPECT_EQ(decoded.rd, 31);                         // XZR
  EXPECT_EQ(decoded.rn, 1);                          // X1
  EXPECT_EQ(decoded.rm, 2);                          // X2
  EXPECT_TRUE(decoded.is64Bit);
  EXPECT_TRUE(decoded.setFlags);
}
