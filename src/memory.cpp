#include "memory.h"

Memory::Memory(size_t size) {
  storage.resize(size, 0); // allocate 'size' bytes, init to 0
}

uint8_t Memory::readByte(uint64_t address) const {
    if(address >= storage.size()) {
        return 0;
    }
    return storage[address];
}

void Memory::writeByte(uint64_t address, uint8_t value) {
  if(address < storage.size()) {
    storage[address] = value;
  }
}

