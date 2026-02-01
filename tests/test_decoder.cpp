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
