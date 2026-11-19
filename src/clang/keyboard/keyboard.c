#include <keyboard.h>
#include <scancodes.h>
#include <kstdlib.h>
#include <idt.h>
#include <ring_buffer.h>

DEFINE_RING_BUFFER(uint8_t, UCharRingBuffer)

static uint8_t storage[128];
static UCharRingBuffer buffer;

void keyboard_init(void)
{
    UCharRingBuffer_init(&buffer, storage, sizeof storage);
}

#define KEYBOARD_INPUT_PORT 0x64
#define KEYBOARD_SCANCODE_PORT 0x60

extern uint8_t inb(uint16_t port);

typedef struct {
    uint64_t time;
    bool extended;
    bool shift;
    bool ctrl;
    bool alt;
} KEYBOARD_STATE;

KEYBOARD_STATE keyb_state;

char scancode_to_ascii[128] = {
    0,    0,   '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '\'', '+',  0,   0,   'q', 'w', 'e', 'r',
    't', 'z', 'u', 'i', 'o', 'p', '[', ']',  0,   0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'','`',  0,  '\\','y', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '-',  0,  '*',  0,  ' ',  0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0
};

char scancode_to_ascii_shift[128] = {
    0,    0,   '!', '"', '#', '$', '%', '&', '/', '(',
    ')', '=', '?', '*',  0,   0,   'Q', 'W', 'E', 'R',
    'T', 'Z', 'U', 'I', 'O', 'P', '[', ']',  0,   0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',
    '\'','`',  0,  '\\','Y', 'X', 'C', 'V', 'B', 'N',
    'M', ';', ':', '_',  0,  '*',  0,  ' ',  0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0
};

char scancode_to_ascii_alt[128] = {
    0,    0,   '!', '"', '#', '$', '%', '&', '/', '(',
    ')', '=', '?', '*',  0,   0,   '\\', '|', ' ',  ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',  0,   0,
    ' ', ' ', ' ', '[', ']', ' ', ' ', ' ', ' ', ' ',
    ' ',' ',  0,  '\\',' ', ' ', ' ', '@', '{', '}',
    ' ', '<', '>', ' ',  0,  '*',  0,  ' ',  0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0
};

unsigned char decode_scancode(uint8_t scancode)
{
    char c;

    if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
        keyb_state.shift = true;
        return '\0';
    } if (scancode == KEY_LSHIFT_DOWN || scancode == KEY_RSHIFT_DOWN) {
        keyb_state.shift = false;
        return '\0';
    }

    if (scancode == KEY_CTRL) {
        keyb_state.ctrl = true;
        return '\0';
    } if (scancode == KEY_CTRL_DOWN) {
        keyb_state.ctrl = false;
        return '\0';
    }

    if (scancode == KEY_ALT) {
        keyb_state.alt = true;
        return '\0';
    } if (scancode == KEY_ALT_DOWN) {
        keyb_state.alt = false;
        return '\0';
    }

    if (scancode == KEY_ENTER) return '\n';
    if (scancode == KEY_BKSP) return '\b';

    else if (scancode < 0x3a) {
        if (keyb_state.shift) c = scancode_to_ascii_shift[scancode];
        else if (keyb_state.alt) c = scancode_to_ascii_alt[scancode];
        else c = scancode_to_ascii[scancode];
    } else {
        return scancode;
    }
    return c;
}

/* Polling verzija: čita port direktno i krade scancodeove ISR-u.
   Zovi je samo dok IRQ1 nije upaljen, ili je obriši ako je ne koristiš. */
size_t kinput_b(char *s)
{
    size_t count = 0;
    char c = ' ';
    uint8_t scancode = 0;
    while (c != '\n') {
        if ((inb(KEYBOARD_INPUT_PORT) & 1)) {           // There is scancode waiting
            scancode = inb(KEYBOARD_SCANCODE_PORT);     // Read scancode

            if (scancode & 0x80) continue;
            if (scancode == KEY_ENTER) c = '\n';             // If scancode is enter put \n in c
            else if (scancode < 0x3a) c = scancode_to_ascii[scancode];      // if scancode is a character

            s[count] = c;               // put the character into the string
            count++;                    // increment index of string
        }
    }

    s[count] = '\0';
    return count;
}

char kgetchar_nb()
{
    uint8_t c;
    bool ok;

    asm volatile("cli" ::: "memory");
    ok = UCharRingBuffer_pop(&buffer, &c);
    asm volatile("sti" ::: "memory");

    return ok ? (char)c : '\0';
}

void keyboard_handler()
{
    if (!(inb(KEYBOARD_INPUT_PORT) & 1)) return;

    uint8_t scancode = inb(KEYBOARD_SCANCODE_PORT);

    if (scancode == KEY_EXTENDED) {
        keyb_state.extended = true;
    } else {
        /* decode uvijek zovemo, da shift/ctrl/alt ažuriraju stanje i na release */
        char c = (char)decode_scancode(scancode);

        /* u bafer idu samo pritisci stvarnih znakova, ne release kodovi ni modifikatori */
        if (!(scancode & 0x80) && c != '\0')
            UCharRingBuffer_append(&buffer, (uint8_t)c);
    }
}