Mini RISC-V ISS with SystemC + TLM

Overview
This project is a minimal RISC-V Instruction Set Simulator (ISS) built using SystemC and TLM-2.0.
It demonstrates how a simple RISC-V CPU core communicates with memory through a TLM bus using b_transport.

+-------------+     +---------+     +-----------+
| RISC-V CPU  | --> | TLM Bus | --> |  Memory   |
| (Initiator) |     | Router  |     | (Target)  |
+-------------+     +---------+     +-----------+

Example Program

Assembly:
addi x1, x0, 2
addi x2, x0, 3
add  x3, x1, x2