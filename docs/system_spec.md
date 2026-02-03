# AArch Micro-Simulator System Spec
# AArch64 Simulator - System Specification

## 1. Supported Instruction Set
The simulator currently supports a subset of the ARMv8-A ISA, focusing on integer arithmetic, memory access, and comparison.

### 1.1. Data Processing (Immediate)
* **Encodings:** `ADD` (Immediate), `SUB` (Immediate), `SUBS` (Immediate).
* **Aliases:** `CMP` (Immediate) $\rightarrow$ Alias for `SUBS XZR, Xn, #imm`.
* **Features:**
    * 32-bit (`Wn`) and 64-bit (`Xn`) operations.
    * Zero Register (`XZR`) handling for operands.
    * Flag updates (`N`, `Z`, `C`, `V`) for `SUBS`/`CMP`.

### 1.2. Load and Store (Immediate)
* **Encodings:** `LDR` (Register), `STR` (Register).
* **Addressing Modes:**
    | Mode | Syntax | Behavior | Writeback |
    | :--- | :--- | :--- | :--- |
    | **Unsigned Offset** | `[Xn, #imm]` | Address = `Xn + imm` | No |
    | **Pre-Index** | `[Xn, #imm]!` | Address = `Xn + imm` | Yes (Update `Xn`) |
    | **Post-Index** | `[Xn], #imm` | Address = `Xn` | Yes (Update `Xn` after access) |
* **Stack Pointer:**
    * Base register `X31` is treated as `SP`.
    * Supports stack push/pop via Pre/Post-indexing.

### 1.3. Data Processing (Register)

* **Encodings:** `SUBS` (Register), `CMP` (Register).
* **Features:**
  * Register-to-Register arithmetic.
  * Flag updates for conditional branching support.

## 2. Register Model

* **General Purpose:** `X0` - `X30`.
* **Register 31 (Context Sensitive):**
  * **As Source/Dest (Data):** Read as `0` (`XZR`), Write is ignored.
  * **As Base (Memory/Math):** Read/Write as `SP` (Stack Pointer).

## 3. Memory Model

* **Storage:** Byte-addressable `std::vector<uint8_t>`.
* **Access:** Little-endian read/write helpers (`read64`, `write64`).
