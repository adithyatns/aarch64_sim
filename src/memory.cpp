#include "memory.h"

namespace {
// Naming constants makes the bit-masks readable
constexpr uint32_t BYTES_IN_64BITS = 8; // 10001
constexpr uint32_t MASK_BYTE = 0xFF;    // 5 bits
} // namespace

Memory::Memory(size_t size) {
  storage.resize(size, 0); // allocate 'size' bytes, init to 0
}

auto Memory::readByte(uint64_t address) const -> uint8_t {
  if (address >= storage.size()) {
    return 0;
  }
  return storage[address];
}

void Memory::writeByte(uint64_t address, uint8_t value) {
  if (address < storage.size()) {
    storage[address] = value;
  }
}

auto Memory::read64(uint64_t address) const -> uint64_t {
  // bound check
  if ((address + BYTES_IN_64BITS) > storage.size()) {
    return 0;
  }
  // make sure give base address is low address : little endian
  uint64_t value = 0;
  for (int i = 0; i < BYTES_IN_64BITS; i++) {
    value |= (((uint64_t)storage[address + i]) << (i * BYTES_IN_64BITS));
  }
  return value;
}

void Memory::write64(uint64_t address, uint64_t value) {
  // Bound check
  if (address + BYTES_IN_64BITS > storage.size()) {
    return;
  }

  // Little Endian : reconstruct the 64-bit word
  for (int i = 0; i < BYTES_IN_64BITS; ++i) {
    storage[address + i] = (value >> (i * BYTES_IN_64BITS)) & MASK_BYTE;
  }
}
