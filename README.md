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
- `exit` - Exits shell, and eventually halts the machine
- `pong` - Opens pong
- `cls` - clears screen

## Pong

Since there is not an IDT in place, pong uses polling and a `tick` variable in the game loop.   
At the start, you are asked to choose if you want to play against an AI. Writing `ai` will choose to play against an AI (the AI currently tracks the y position of the ball, so you could also call it a DI - dumb intelligence xD).   

Note: when choosing to play between an AI or 2 player mode, your input is not visible.

## Future additions

Here is a list of features i plan to add:

- Interrupts
- Kernel/User mode (ring0 and ring3)
- System calls (`syscall`/`int 0x80`)
- FAT 12/16 filesystem support
