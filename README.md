# AKOS

## Description

AKOS is a 32 bit operating system. 

## How to run

To run AKOS, you need to:

- clone the repo
- make `./compiled/bin` and `compiled/obj` directories
- run make

Note: Should be done in a Linux environment. The compiler used is GNU GCC, the folders `clang` and `clangh` have nothing to do with the clang compiler.

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
