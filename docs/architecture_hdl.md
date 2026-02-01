# AArch64 Simulator: High-Level model Design (HLD)

## System Overview

This simulator is functional model of ARMv8-A A64 instruction set. It aims to execute the binary compiled for aarch64

## 2. Architecture Components

* **CPUState:** Manages the 31 general-purpose registers (X0-X30), the Stack Pointer (SP), and the Program Counter (PC).
* **Instruction Decoder:** A hierarchical bit-masking engine that identifies instructions.
* **Execution Engine:** Implements the ALU logic for data processing and branch control.
* **Memory** Implements a byte-addressable RAM storage (`std::vector<uint8_t>`) with support for Little Endian 64-bit word access (`read64`/`write64`).

## 3. Instruction Lifecycle

The simulator operates in a loop:

1. Fetch 32-bit word from Memory[PC].
2. Decode bit-fields using masks.
3. Execute operation and update Registers/Flags.
4. PC = PC + 4.

## 4. Instruction Mapping (In Progress)

### 4.1 Data processing - Immediate

pg790 - arm reference manual

#### 4.1.1 Add/Sub immediate

| Field | Bits | Description |
| :--- | :--- | :--- |
| sf | [31] | 64-bit flag |
| op | [30] | add/sub bit |
| S | [29] | op with update condition flag bit |
| 100010 | [28-23] | 6 bit Add/Sub imm op code |
| sh | [22] | shift bit |
| imm12 | [21:10] | 12-bit unsigned immediate |
| rn | [9-5] | Source 32 bit CPU register encoded 5 bit |
| rd | [4-0] | destination 32 but CPU register encoded 5 bit |

### 5. Execution Pipeline

The simulation follows a decoupled design to ensure testability:

1. **Decoder** Pure function that maps 32-bit hex to
2. **Executor**
3. **CPUState**

### Supported Instruction

* `ADD` (Immediate)
* `SUB` (Immediate)
* `LDR` (Immediate)
* `STR`

### LDR (Unsigned Immediate)

The instruction 0xF9400420 corresponds to Load Register (Unsigned Immediate).

## AArch64 Instruction Implementation Reference

### 1. Data Processing (Immediate)

These instructions perform arithmetic using a Register and an Immediate value.

**Common Encoding Pattern:**
`[sf] [op] [S] 10001 0 [shift] [imm12] [Rn] [Rd]`

* **sf (31):** Size Flag. `0` = 32-bit (`W`), `1` = 64-bit (`X`).
* **op (30):** Operation. `0` = ADD, `1` = SUB.
* **S (29):** Set Flags. `0` = No flags (ADD/SUB), `1` = Update PSTATE (ADDS/SUBS).
* **shift (22):** `0` = LSL #0, `1` = LSL #12.

#### ADD (Immediate)

Adds an immediate value to a register.

* **Mnemonic:** `ADD <Xd|Wd>, <Xn|Wn>, #<imm>`
* **Bit Pattern (64-bit):** `1 0 0 10001 0 [shift] [imm12] [Rn] [Rd]`
* **Implementation Logic:**

    ```cpp
    uint64_t op1 = cpu.x[rn];
    uint64_t op2 = (shift == 1) ? (imm12 << 12) : imm12;
    cpu.x[rd] = op1 + op2;
    // Note: Does NOT update NZCV flags.
    ```

#### SUB (Immediate)

Subtracts an immediate value from a register.

* **Mnemonic:** `SUB <Xd|Wd>, <Xn|Wn>, #<imm>`
* **Bit Pattern (64-bit):** `1 1 0 10001 0 [shift] [imm12] [Rn] [Rd]`
* **Implementation Logic:**

    ```cpp
    uint64_t op1 = cpu.x[rn];
    uint64_t op2 = (shift == 1) ? (imm12 << 12) : imm12;
    cpu.x[rd] = op1 - op2;
    // Note: Does NOT update NZCV flags.
    ```

---

### 2. Loads and Stores (Unsigned Immediate)

These instructions transfer data between registers and memory using a base address plus a positive offset.

**Common Encoding Pattern:**
`[size] 111001 [opc] [imm12] [Rn] [Rt]`

* **size (31:30):** `00`=Byte, `01`=Halfword, `10`=Word, `11`=Double(64-bit).
* **opc (23:22):** `00` = Store (`STR`), `01` = Load (`LDR`).
* **imm12 (21:10):** The block offset (Must be scaled!).

#### LDR (Load Register Immediate)

Loads a register from memory.

* **Mnemonic:** `LDR <Xt|Wt>, [<Xn|SP>, #<pimm>]`
* **Bit Pattern (64-bit):** `11 111001 01 [imm12] [Rn] [Rt]`
* **Critical Scaling Logic:**
    The `imm12` field is **not** bytes. It is a block count.
  * Size `00` (Byte): Offset = `imm12`
  * Size `11` (64-bit): Offset = `imm12 << 3` (Multiply by 8)

* **Implementation Logic:**

    ```cpp
    uint64_t offset = imm12 << size; // Dynamic scaling based on size bits
    uint64_t address = (rn == 31 ? cpu.sp : cpu.x[rn]) + offset;
    cpu.x[rt] = memory.read64(address);
    ```

#### STR (Store Register Immediate)

Stores a register to memory.

* **Mnemonic:** `STR <Xt|Wt>, [<Xn|SP>, #<pimm>]`
* **Bit Pattern (64-bit):** `11 111001 00 [imm12] [Rn] [Rt]`
* **Scaling Logic:** Same as LDR (Scale by register size).
* **Implementation Logic:**

    ```cpp
    uint64_t offset = imm12 << size;
    uint64_t address = (rn == 31 ? cpu.sp : cpu.x[rn]) + offset;
    uint64_t data = (rt == 31 ? 0 : cpu.x[rt]); // XZR check for source
    memory.write64(address, data);
    ```

## 1. Behavioral Differences

| Mode | Syntax | Behavior | C++ Logic Equivalent |
| :--- | :--- | :--- | :--- |
| **Offset** | `[Xn, #imm]` | Access memory at `Xn + imm`. **Do not** change `Xn`. | `addr = Xn + imm;` `val = mem[addr];` |
| **Pre-Index** | `[Xn, #imm]!` | Update `Xn = Xn + imm` **before** access. | `Xn = Xn + imm;` `val = mem[Xn];` |
| **Post-Index** | `[Xn], #imm` | Access memory at `Xn`, **then** update `Xn = Xn + imm`. | `addr = Xn;` `Xn = Xn + imm;` `val = mem[addr];` |

---

## 2. Decoding Logic (How to Distinguish)

For standard Load/Store Immediate instructions (where Bits 29:27 = `111` and Bit 25 = `0`), the mode is determined by **Bit 24** and **Bits 11:10**.

### Step A: Check Bit 24 (Unsigned Flag)

* **If Bit 24 == `1`:**
  * **Mode:** `AddrMode::Offset` (Unsigned Offset).
  * **Logic:** The immediate is positive and scaled by the operation size (e.g., `imm12 << 3` for 64-bit).

* **If Bit 24 == `0`:**

  * It is an **Indexed** or **Unscaled** operation. You must check Bits 11:10.

### Step B: Check Bits 11:10 (Mode Bits)

*(Only valid if Bit 24 was `0`)*

| Bits 11:10 | Mode | Description |
| :--- | :--- | :--- |
| **`00`** | `AddrMode::Offset` | **Unscaled Signed Offset.** (e.g., `[Xn, #-8]`). No writeback. |
| **`01`** | `AddrMode::PostIndex` | **Post-Index.** Update base register *after* access. |
| **`11`** | `AddrMode::PreIndex` | **Pre-Index.** Update base register *before* access. |
| **`10`** | *Restricted* | Typically unprivileged access (often treated as Offset in simple sims). |

---

## 3. C++ Implementation Snippet

Here is how to structure this inside your `Decoder::decode` function:

```cpp
void decode_load_store(uint32_t instr, DecodedInstruction& decoded) {
    // 1. Check Bit 24 (Is it the standard Unsigned Offset?)
    bool is_unsigned_offset = (instr >> 24) & 1;

    if (is_unsigned_offset) {
        // --- Case 1: Standard Offset ---
        decoded.mode = AddrMode::Offset;

        // Decode and Scale Immediate
        uint32_t imm12 = (instr >> 10) & 0xFFF;
        uint8_t size = (instr >> 30) & 0x3;
        decoded.imm = imm12 << size;
    }
    else {
        // --- Case 2: Indexed / Unscaled Modes ---
        // Extract 9-bit Signed Immediate
        int32_t imm9 = (instr >> 12) & 0x1FF;
        if (imm9 & 0x100) imm9 |= 0xFFFFFE00; // Sign Extension
        decoded.imm = imm9;

        // Check Bits 11:10 for specific mode
        uint8_t mode_bits = (instr >> 10) & 0x3;

        switch (mode_bits) {
            case 0b01: decoded.mode = AddrMode::PostIndex; break;
            case 0b11: decoded.mode = AddrMode::PreIndex; break;
            default:   decoded.mode = AddrMode::Offset; break; // 0b00 is Unscaled
        }
    }
}
```

---

### 3. Summary Implementation Table

| Mnemonic | Class (Bits 28:25) | Opcode Distinctions | Immediate Handling |
| :--- | :--- | :--- | :--- |
| **ADD** | `1000` or `1001` | Bit 30 = `0` | `imm12 << (bit22 ? 12 : 0)` |
| **SUB** | `1000` or `1001` | Bit 30 = `1` | `imm12 << (bit22 ? 12 : 0)` |
| **LDR** | `x1x0` (e.g. `1110`) | Bit 22 = `1` | `imm12 << size` |
| **STR** | `x1x0` (e.g. `1110`) | Bit 22 = `0` | `imm12 << size` |
