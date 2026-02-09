#pragma once
#include <cstdint>
#include <iostream>

/**
 * @brief A simple decoder for a subset of AArch64 instructions, including:
 * - Data Processing (Immediate): ADD, SUB
 * - Data Processing (Register): ADD, SUB
 * - Load/Store (Immediate): LDR, STR with various addressing modes
 * - Branches: B, BL, B.cond
 */
enum class InstructionType {
  UNKNOWN,
  ADD_IMM,
  SUB_IMM,
  ADD_REG,
  SUB_REG,
  LDR,
  STR,
  BRANCH,
  BRANCH_COND,
};

/**
 * @brief Addressing modes for load/store instructions. For ADD/SUB, this will
 * be None.
 * - Offset: [Xn, #imm]
 * - PreIndex: [Xn, #imm]!
 * - PostIndex: [Xn], #imm
 */
enum class AddrMode {
  None,     // For Math (ADD/SUB)
  Offset,   // [Xn, #imm]
  PreIndex, // [Xn, #imm]!
  PostIndex // [Xn], #imm
};
/**
 * @brief DecodedInstruction struct to hold the decoded information from a
 * 32-bit instruction.
 * - type: The type of instruction (e.g., ADD_IMM, LDR,
 * BRANCH_COND, etc.)
 * - rd: Destination register (0-31)
 * - rn: First source register (0-31)
 * - rm: Second source register (0-31), used for register-based instructions
 * like SUB_REG
 * - imm: Immediate value, sign-extended if necessary
 * - mode: Addressing mode for load/store instructions
 * - is64Bit: Indicates if the instruction operates on 64-bit registers (true)
 * or 32-bit registers (false)
 * - setFlags: For CMP instructions, indicates if flags should be set (true if
 * rd is XZR)
 * - cond: Condition code for conditional branches (0-15), valid only if type is
 * BRANCH_COND
 */
struct DecodedInstruction {
  InstructionType type = InstructionType::UNKNOWN;
  uint8_t rd = 0;
  uint8_t rn = 0;
  uint8_t rm = 0; // For SUB_REG
  int16_t imm = 0;
  AddrMode mode = AddrMode::None;
  bool is64Bit = false;
  bool setFlags = 0; // For CMP instructions
  uint8_t cond = 0;  // For conditional branches
};

/**
 * @brief Decoder class with a static method to decode a 32-bit instruction into
 * a DecodedInstruction struct. The decode method identifies the instruction
 * type, extracts relevant fields (rd, rn, rm, imm), determines the addressing
 * mode for load/store instructions, and sets flags for conditional branches and
 * CMP instructions. The decoder focuses on a specific subset of AArch64
 * instructions, making it suitable for educational purposes or as a starting
 * point for a more comprehensive decoder. The implementation uses bit
 * manipulation to extract fields from the instruction and relies on predefined
 * constants for instruction groups and bit positions. It also includes handling
 * for sign-extension of immediate values and distinguishes between 32-bit and
 * 64-bit operations based on specific bits in the instruction. Note: This
 * decoder is not exhaustive and only supports a limited set of instructions. It
 * can be extended to cover more instructions and addressing modes as needed.
 *
 */
class Decoder {
public:
  static auto decode(uint32_t instr) -> DecodedInstruction;
};
