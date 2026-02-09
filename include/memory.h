#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * @brief Memory class to represent the memory of the simulated system. It
 * provides methods to read and write bytes and 64-bit values at specific
 * addresses. The memory is implemented as a vector of bytes, and the class
 * ensures that all accesses are within the allocated size. This class abstracts
 * away the details of memory management and provides a simple interface for the
 * executor to interact with memory during instruction execution. The readByte
 * and writeByte methods allow for byte-level access, while read64 and write64
 * provide convenient methods for accessing 64-bit values, which are common in
 * AArch64 instructions. The class can be extended to include additional
 * functionality such as memory-mapped I/O or support for different endianness
 * if needed. Note: This implementation does not include any bounds checking for
 * simplicity, but it can be added to ensure that all memory accesses are valid
 * and to prevent out-of-bounds errors.
 *
 */
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
