# About

This document conatins some stuff I find useful, like a memory map, or different ASCII characters and such.

## ASCII Characters

Here are some ASCII characters that are useful for TUI-s, or just generally:

| HEX  | NAME               | CHAR |
| :--: | :----------------: | :--: |
| 0xDB | Full-block         |  █   |
| 0xB2 | 75% filled block   |  ▓   |
| 0xB1 | 50% filled block   |  ▒   |
| 0xB0 | 25% filled block   |  ░   |
| 0xDF | Upper half block   |  ▀   |
| 0xDC | Lower half block   |  ▄   |
| 0xBB | Upper right angle  |  ╗   |
| 0xC9 | Upper left angle   |  ╔   | 
| 0xBC | Lower right angle  |  ╝   | 
| 0xC8 | Lower left angle   |  ╚   | 
| 0xBA | Vertical line      |  ║   |
| 0xCD | Horizontal name    |  ═   |

## Memory map

Here is the memory map that the OS uses:

| Starting address |              Purpose              |
| :--------------: | :-------------------------------: |

## VGA colors

When passing a color it is a hex code like `0x0f`, the first byte (leftmost) is the color of font, the second byte is the background 

| Hex  | Boja              |
|------|-------------------|
| 0x00 | Black on Black    |
| 0x01 | Blue on Black     |
| 0x02 | Green on Black    |
| 0x03 | Cyan on Black     |
| 0x04 | Red on Black      |
| 0x05 | Magenta on Black  |
| 0x06 | Brown on Black    |
| 0x07 | Light Grey on Black |
| 0x08 | Dark Grey on Black |
| 0x09 | Light Blue on Black |
| 0x0A | Light Green on Black |
| 0x0B | Light Cyan on Black |
| 0x0C | Light Red on Black |
| 0x0D | Light Magenta on Black |
| 0x0E | Yellow on Black   |
| 0x0F | White on Black    |