#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>

class Memory {
public:
  // constructor : create memory with size given by application in bytes
  Memory(size_t size);

  // reading a byte of specific address from memory
  uint8_t readByte(uint64_t address) const;
  // writing a byte on memory on specific address
  void writeByte(uint64_t address, uint8_t val);

private:
  std::vector<uint8_t> storage;
};

