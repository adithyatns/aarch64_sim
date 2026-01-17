# AArch64 Simulator: High-Level model Design (HLD)

## System Overview
This simulator is functional model of ARMv8-A A64 instruction set. It aims to execute the binary compiled for aarch64

## 2. Architecture Components
* **CPUState:** Manages the 31 general-purpose registers (X0-X30), the Stack Pointer (SP), and the Program Counter (PC).
* **Instruction Decoder:** A hierarchical bit-masking engine that identifies instructions.
* **Execution Engine:** Implements the ALU logic for data processing and branch control.

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
| op | [30] | add/sub bit|
| S  | [29] | op with update condition flag bit |  
| 100010 | [28-23] | 6 bit Add/Sub imm op code|
| sh | [22] | shift bit |
| imm12 | [21:10] | 12-bit unsigned immediate |
| rn |[9-5] | Source 32 bit CPU register encoded 5 bit|
|rd |[4-0]| destination 32 but CPU register encoded 5 bit|
