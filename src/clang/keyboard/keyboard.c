#include <keyboard.h>
#include <scancodes.h>

#define KEYBOARD_INPUT_PORT 0x64
#define KEYBOARD_SCANCODE_PORT 0x60

extern uint8_t inb(uint16_t port);

unsigned char decode_scancode(uint8_t scancode)
{
    char c;
    if (scancode < 0x3a) {
        c = scancode_to_ascii[scancode];
    } else {
        return scancode;
    }
    return c;
}

size_t kinput_b(char *s)
{
    size_t count = 0;
    char c = ' ';
    uint8_t scancode = 0;
    while (c != '\n') {
        if ((inb(KEYBOARD_INPUT_PORT) & 1)) {           // There is scnacode waiting
            scancode = inb(KEYBOARD_SCANCODE_PORT);     // Read scancode

            if (scancode & 0x80) continue;
            if (scancode == KEY_ENTER) c = '\n';             // If scancode is enter put \n in c
            else if (scancode < 0x3a) c = scancode_to_ascii[scancode];      // if scnacode is a character

            s[count] = c;               // put the character into the string
            count++;                    // increment index of string
        }
    }

    s[count] = '\0';
    return count;
}

char kgetchar_nb()
{
    if (!(inb(KEYBOARD_INPUT_PORT) & 1)) return 0;
    return decode_scancode(inb(KEYBOARD_SCANCODE_PORT));
}

