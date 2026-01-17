#include "registers.h"

namespace arm64 {

auto CPUState::getReg(uint8_t regId) const -> uint64_t {
  if (regId == REG_XZR) {
    return 0; // The Hardware "Zero Wire"
  }
  return X.at(regId); // .at() checks bounds, safer than []
}

auto CPUState::setReg(uint8_t regId, uint64_t value) -> void {
  if (regId == REG_XZR) {
    return; // Writes to XZR are discarded (black hole)
  }
  X.at(regId) = value;
}

} // namespace arm64
