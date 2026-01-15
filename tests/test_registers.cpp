#include "registers.h"
#include <gtest/gtest.h>

using namespace arm64;

TEST(RegisterTest, ZeroRegisterAlwaysZero) {
  CPUState cpu;
  cpu.X[0] = 0xDEADC0DE; // Set a value

  // Logic check: reading the zero register should always return 0
  EXPECT_EQ(cpu.readXZR(), 0);
}

TEST(RegisterTest, PCStartsAtZero) {
  CPUState cpu = {}; // Zero initialize
  EXPECT_EQ(cpu.PC, 0);
}
