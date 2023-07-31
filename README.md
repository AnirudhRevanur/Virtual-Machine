# Virtual Machine in C 

This repository contains a simple implementation of a Virtual Machine in C, designed to mimic the behavior of the LC3 architecture. The LC3 is a simple educational computer architecture used for teaching purposes, and this virtual machine provides a way to execute LC3 assembly code on a modern computer.

## Features

- LC3 Instruction Set: The virtual machine supports a subset of the LC3 instruction set, including arithmetic operations, memory access, control flow, and I/O instructions.
- Batch Mode: It can execute pre-written LC3 assembly code from a file.

## Getting Started

### Prerequisites

Make sure you have the following installed on your system:

- GCC (GNU Compiler Collection) or any C compiler supporting C99.
- [Optional] A text editor or IDE of your choice.

### Installation

1. Clone the repository to your local machine:

   ```bash
   git clone https://github.com/AnirudhRevanur/Virtual-Machine.git
   cd Virtual-Machine
   ```

2. Compile the virtual machine code:

   ```bash
   gcc -o VirtualMachine lc3v2.c
   ```

## Usage

We have provided some object files using which you can run the virtual machine in the images directory.

To execute the `2048.obj` program:

```bash
./VirtualMachine ./images/2048.obj
```

And to execute the `rogue.obj` program:

```bash
./VirtualMachine ./images/rogue.obj
```

The virtual machine will read the image file, execute the instructions, and display the output.



## Supported Instructions

The virtual machine supports a subset of the LC3 instruction set, including:

- ADD, AND, NOT, BR (Branch), JMP (Jump), JSR (Jump to Subroutine), LDR (Load Register), LD (Load), LEA (Load Effective Address), STR (Store), ST (Store), RTI (Return from Interrupt), TRAP (Trap Routine), etc.

---