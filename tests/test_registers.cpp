#include "registers.h"
#include <gtest/gtest.h>

using namespace arm64;

TEST(RegisterTest, BasicReadWrite) {
  arm64::CPUState cpu;
  cpu.setReg(0, 42);
  EXPECT_EQ(cpu.getReg(0), 42);
}

TEST(RegisterTest, ZeroRegisterIsAlwaysZero) {
  arm64::CPUState cpu;

  // Try to write to XZR (Register 31)
  cpu.setReg(31, 0xDEADBEEF);

  // Read back - MUST be 0
  EXPECT_EQ(cpu.getReg(31), 0);
}

TEST(RegisterTest, RegisterStateIndependence) {
  arm64::CPUState cpu;
  cpu.setReg(0, 100);
  cpu.setReg(1, 200);

  // Ensure writing X1 didn't overwrite X0
  EXPECT_EQ(cpu.getReg(0), 100);
  EXPECT_EQ(cpu.getReg(1), 200);
}
