#include <shell.h>
#include <keyboard.h>
#include <kstdlib.h>

typedef void (*program_t)();

void shell(program_t *programs[], char *names[], size_t count)
{
	// TODO: add the programs[] support
	char command[128] = {0};
	unsigned char c = ' ';
	int index = 0;
	char text[2] = {0};

	char *help_message[5] = {
		"EXIT - Exit the shell\n",
		"CLS - Clear the screen\n",
		"UNAME - Show version number\n",
		"HELP - Show help message\n",
	};

	kprint("> ");

	while (1) {
		c = kgetchar_nb();
		if (c != 0 && (index < 126 || c == '\n')) {
			if (c < 0x80 && c >= 0x20) {
				text[0] = c;
				text[1] = '\0';
				kprint(text);
				command[index++] = c;
			} else if (c == '\n') {
				kprint("\n");
				command[index++] = '\0';
				if (kstrcmp("exit", command) == true) {
					return;
				} else if (kstrcmp("cls", command) == true) {
					kclear_vga_buffer();
					kprint("\n");
				} else if (kstrcmp("uname", command) == true) {
					kprint("AKOS(C) Kernel v0.1\n");
				} else if (kstrcmp("help", command) == true) {
					for (int i = 0; i < 4; i++) kprint(help_message[i]);
				}

				zero_memory((address)command, (address)command + index);
				index = 0;
				kprint("> ");
			}
			// kprint("\nCMD: ", vga_args);
			// kprint(command, vga_args);
			// kprint("\n", vga_args);
		}
	}
}