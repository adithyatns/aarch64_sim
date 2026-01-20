#include <gtest/gtest.h>
#include "memory.h"

TEST(MemoryTest, BasicReadWrite) {
  // create a 1kb of RAM 
  Memory ram(1024);
  // write 0xFF to address 0x100
  ram.writeByte(100, 0xff);
  // expect the value at 0x100 to 0xFF check by reading it
  EXPECT_EQ(ram.readByte(100), 0xFF);
}

TEST(MemoryTest, OutofBoundSafe) {
  Memory ram(1024);

  EXPECT_EQ(ram.readByte(9999), 0);
}
