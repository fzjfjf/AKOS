# AKOS

## Description

AKOS is a 32 bit operating system. 

## How to run

### Prerequisites

To build AKOS, you need to have:

- GNU GCC Compiler
- NASM
- LD
- qemu-system-i386 or qemu-system-x86_64

### Building

To build AKOS, you need to:

- clone the repo
- make `./compiled/bin` and `compiled/obj` directories
- run make

Note: Should be done in a Linux environment. The compiler used is GNU GCC, the folders `clang` and `clangh` have nothing to do with the clang compiler.

### Running

After AKOS is built, run `make run`

## Features

AKOS currently supports a shell, which has basic commands like 

- `help` - Shows a list of commands
- `uname` - Shows version
- `reboot` - Reboots machine
- `pong` - Opens pong
- `cls` - clears screen

AKOS supports basic interrupts (keyboard, timer and GPF).  

## Pong 

AI Mode is currently borked (i think)

Note: when choosing to play between an AI or 2 player mode, your input is not visible.

## Future additions

Here is a list of features i plan to add:

- Kernel/User mode (ring0 and ring3)
- System calls (`syscall`/`int 0x80`)
- FAT 32 filesystem support (in progress)
