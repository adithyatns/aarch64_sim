#pragma once

class CPU {
private:
  uint64_t register[32];
  Memory &mem;

public:
  CPU(Memory &memory) : mem(memory) {}
};
