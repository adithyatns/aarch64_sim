#include "decoder.h"
#include <gtest/gtest.h>

TEST(DecoderTest, DecodeImmidiate_ADD) {
  // Binary for: ADD XO, X1, #5
  // 0b 1 0 0 1.0001.0 0 00.0000.0001.01 00.001 0.0000
  //   sf ops opcode   sh imm12          Rn     Rd
  uint32_t instrHex = 0x91001420;

  auto result = Decoder::decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::ADD_IMM);
  EXPECT_EQ(result.rd, 0);  // X0
  EXPECT_EQ(result.rn, 1);  // X1
  EXPECT_EQ(result.imm, 5); // 5

  EXPECT_TRUE(result.is64Bit);
}

TEST(DecoderTest, DecodeImmidiate_SUB) {
  // Binary for: SUB XO, X1, #5 - 64 bit
  // 0b 1 1 0 1.0001.0 0 00.0000.0001.01 00.001 0.0000
  //   sf ops opcode   sh imm12          Rn     Rd
  uint32_t instrHex =
      0xD1001420; // 0b 1 1 0 100 01 0000 0000 0001 0100 0010 0000

  auto result = Decoder::decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::SUB_IMM);
  EXPECT_EQ(result.rd, 0);
  EXPECT_EQ(result.rn, 1);
  EXPECT_EQ(result.imm, 5);

  EXPECT_TRUE(result.is64Bit);
}

TEST(DecoderTest, DecodeSubImmediate_SUB2) {
  uint32_t instrHex = 0xD1002862; // SUB X2, X3, #10

  auto result = Decoder::decode(instrHex);

  EXPECT_EQ(result.type, InstructionType::SUB_IMM);
  EXPECT_EQ(result.rd, 2);   // X2
  EXPECT_EQ(result.rn, 3);   // X3
  EXPECT_EQ(result.imm, 10); // #10
  EXPECT_TRUE(result.is64Bit);
}
