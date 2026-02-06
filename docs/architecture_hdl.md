# AArch64 Simulator - High-Level Design (HLD)

## 1. System Overview

The simulator is a modular, cycle-accurate-style functional emulator for the ARMv8 AArch64 instruction set. It decouples Instruction Decoding from Execution and State Management.

## 2. Core Components

### 2.1. CPU State (`arm64::CPUState`)

* **Registers:** 31 General Purpose Registers (`X0-X30`).
* **Special Registers:**
  * `SP` (Stack Pointer): Handled distinctly from `X31`.
  * `PC` (Program Counter): 64-bit instruction pointer.
* **PSTATE (Flags):**

  * Stores `N` (Negative), `Z` (Zero), `C` (Carry), `V` (Overflow).
  * Updated by flag-setting instructions (e.g., `SUBS`, `CMP`).

### 2.2. Decoder (`Decoder` Class)

The decoder uses a hierarchical bit-masking strategy to classify instructions.

* **Top-Level Dispatch:** Uses bits `[28:25]` to route instructions to specific groups.
  * `100x`: Data Processing - Immediate.
  * `x1x0`: Loads and Stores.
  * `101x`: Branches and System.
  *
* **Decoding Logic:**

* **Data Processing:** Extracts `sf` (size), `op` (opcode), and `imm12` (immediate).
* **Load/Store:** Identifies Addressing Mode based on bits `24` and `11:10`.
* **Modes:** `Offset`, `PreIndex`, `PostIndex`.
* **Branches:** Distinguishes between Unconditional (`B`) and Conditional (`B.cond`).

### 2.3. Executor (`Executor` Class)

The executor is stateless and acts upon the `CPUState` and `Memory`.

* **Register Abstraction:**

* Uses private helpers `read_reg` and `write_reg` to resolve Register 31 ambiguity.
* **Context:** `is_sp` flag determines if Reg 31 is `SP` (Stack Pointer) or `XZR` (Zero Register).
* **Instruction Handling:**
* **ALU Operations:** Performs arithmetic (`ADD`, `SUB`) and updates PSTATE flags (`SUBS`/`CMP`).
* **Memory Operations:** Calculates Effective Address based on `AddrMode`. Handles Writeback for Pre/Post-Index modes.
* **Branch Operations:** Evaluates PSTATE conditions (EQ, NE, etc.) and updates `PC`.

## 3. Implementation Status

| Instruction Group | Mnemonic | Bits 28:25 | Opcode / Distinctions | Status | Notes |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Data Processing (Imm)** | `ADD` | `100x` | Bit 30=0, S=0 | ✅ **Done** | Updates `Rd`. |
| | `SUB` | `100x` | Bit 30=1, S=0 | ✅ **Done** | Updates `Rd`. |
| | `SUBS` | `100x` | Bit 30=1, S=1 | ✅ **Done** | Updates `PSTATE` (NZCV). |
| | `CMP` | `100x` | Alias of `SUBS` | ✅ **Done** | Discards result. |
| **Load / Store** | `LDR` | `x1x0` | Bit 22=1 | ✅ **Done** | Offset, Pre-Index, Post-Index. |
| | `STR` | `x1x0` | Bit 22=0 | ✅ **Done** | Offset, Pre-Index, Post-Index. |
| **Branch (Immediate)** | `B` | `0001` | Bits 31-26=`000101` | ✅ **Done** | Unconditional (`PC + imm26`). |
| | `B.cond | `0101` | Bits 31-24=`01010100` | ✅ **Done** | Conditional (`PC + imm19`). |
| **System** | `NOP` | `0000` | All Zeros | ❌ *Pending* | - |

## 4. Data Flow

1. **Fetch:** (Future) Retrieve instruction from Memory at `PC`.
2. **Decode:** `Decoder::decode(uint32_t)` returns a `DecodedInstruction` struct.
3. **Execute:** `Executor::execute(instr, cpu, mem)` performs the operation and updates state.
