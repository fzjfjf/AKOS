#include <shell.h>
#include <keyboard.h>
#include <kstdlib.h>

typedef void (*program_t)();

void shell(program_t *programs[], char *names[], size_t count, VGA_t *vga_args)
{
	char command[128] = {0};
	unsigned char c = ' ';
	int index = 0;
	char text[2] = {0};

	kprint("> ", vga_args);

	while (1) {
		c = kgetchar_nb();
		if (c != 0) {
			if (c < 0x80 && c >= 0x20) {
				text[0] = c;
				text[1] = '\0';
				kprint(text, vga_args);
				command[index++] = c;
			} else if (c == '\n') {
				kprint("\n", vga_args);
				command[index++] = '\0';
				if (kstrcmp("exit", command, vga_args) == true) {
					return;
				}

				zero_memory((address)command, (address)command + index);
				index = 0;
				kprint("> ", vga_args);
			}
			// kprint("\nCMD: ", vga_args);
			// kprint(command, vga_args);
			// kprint("\n", vga_args);
		}


	}
}