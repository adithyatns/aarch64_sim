#pragma once
#include "decoder.h"
#include "registers.h"

class Executor {
public:
  // Takes the decoded instruction and updates the CPU state accordingly
  static auto execute(const DecodedInstruction &instr, arm64::CPUState &cpu)
      -> void;
};
