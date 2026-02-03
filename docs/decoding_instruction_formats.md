# AArch64 Simulator: Decoding & Instruction Formats

## 1. Overview

This document outlines the bit-manipulation strategy used to identify and decompose AArch64 instructions. The logic strictly follows the **ARMv8-A Architecture Reference Manual (ARM DDI 0487)**.

## 2. Top-Level Instruction Encoding

**Reference:** *C4.1 A64 instruction set encoding*

AArch64 uses a fixed 32-bit instruction length. The primary decode switch is based on bits **[28:25]**.

| Bits [28:25] | Group | Simulator Status |
| :--- | :--- | :--- |
| `x1x0` | **Loads and Stores** | ✅ Implemented |
| `100x` | **Data Processing - Immediate** | ✅ Implemented |
| `101x` | **Branches, Exceptions, System** | ⏳ Planned |
| `x101` | **Data Processing - Register** | ⏳ Planned |
| `0111` | **SIMD and Floating Point** | ❌ Out of Scope |

---

## 3. Implemented Instruction Classes

### 3.1. Load/Store Register (Immediate)

**Reference:** *C4.1.4 Loads and Stores* -> *Load/store register (immediate)*

These instructions transfer data between registers and memory using a base address and an immediate offset.

#### Bit Format

| 31:30 | 29:27 | 26 | 25 | 24 | 23:22 | 21:10 | 9:5 | 4:0 |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `size` | `111` | `V` | `0` | **`0`** | `opc` | `imm9` / `mode` | `Rn` | `Rt` |
| `size` | `111` | `V` | `0` | **`1`** | `opc` | `imm12` | `Rn` | `Rt` |

#### Addressing Mode Logic

The simulator distinguishes addressing modes using **Bit 24** and **Bits [11:10]**.

1. **Unsigned Offset (Shifted):**

    * **Condition:** `Bit 24 == 1`
    * **Decode:** `imm12` is extracted and zero-extended. It is shifted left by the instruction size (byte shift).
    * **Behavior:** `Address = Xn + (imm12 << size)`. `Xn` is **not** updated.

2. **Pre-Index / Post-Index (Unscaled):**

    * **Condition:** `Bit 24 == 0` AND `Bits [11:10] != 00`
    * **Decode:** `imm9` is extracted and signed-extended.
    * **Pre-Index (`11`):** `Xn = Xn + imm9`; Access memory at new `Xn`.
    * **Post-Index (`01`):** Access memory at current `Xn`; then `Xn = Xn + imm9`.

---

### 3.2. Data Processing - Immediate

**Reference:** *C4.1.2 Data Processing - Immediate* -> *Add/subtract (immediate)*

Used for arithmetic with constants (e.g., `ADD X0, X1, #42` or `CMP X0, #42`).

#### Bit Format

| 31 | 30 | 29 | 28:24 | 23:22 | 21:10 | 9:5 | 4:0 |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `sf` | `op` | `S` | `10001` | `shift` | `imm12` | `Rn` | `Rd` |

#### Flag Setting Logic (`SUBS` / `CMP`)

* **Condition:** `op == 1` (Subtract) and `S == 1` (Set Flags).
* **CMP Alias:** If `Rd == 31` (11111), the result is discarded, but PSTATE flags (N, Z, C, V) are updated.
* **Implementation:**

    ```cpp
    bool is_cmp = (instr.type == SUB_IMM) && (instr.rd == 31);
    if (is_cmp || instr.setFlags) {
        cpu.pstate.Z = (result == 0);
        cpu.pstate.N = (result < 0);
        // C and V calculated via carry/overflow logic
    }
    ```

---

### 3.3. Compare & Branch (Immediate)

**Reference:** *C4.1.3 Branches, Exception Generating and System instructions*
*(Upcoming Implementation)*

* **Unconditional Branch (`B`):** Jumps to `PC + imm26`.
* **Conditional Branch (`B.cond`):** Jumps to `PC + imm19` only if specific PSTATE condition flags are met.
