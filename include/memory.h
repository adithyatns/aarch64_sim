#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

class Memory {
public:
  // constructor : create memory with size given by application in bytes
  Memory(size_t size);

  // reading a byte of specific address from memory
  uint8_t readByte(uint64_t address) const;
  // writing a byte on memory on specific address
  void writeByte(uint64_t address, uint8_t val);
  // reading a 8 bytes of specific address from memory
  uint64_t read64(uint64_t address) const;
  // writing a 8 bytes on memory on specific address
  void write64(uint64_t address, uint64_t val);

private:
  std::vector<uint8_t> storage;
};
