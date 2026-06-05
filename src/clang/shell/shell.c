#include <shell.h>
#include <keyboard.h>
#include <kstdlib.h>
#include <pong/pong.h>

void shell(program_t *programs[], char *names[], size_t count)
{
	// TODO: add the programs[] support
	char command[128] = {0};
	unsigned char c = ' ';
	int index = 0;
	char text[2] = {0};

	char *help_message[] = {
		"EXIT - Exit the shell\n",
		"CLS - Clear the screen\n",
		"UNAME - Show version number\n",
		"HELP - Show help message\n",
		"REBOOT - Reboot PC\n",	
		"PONG - Play pong!\n"
	};

	kprint("> ");

	while (1) {
		c = kgetchar_nb();					// get input
		if (c != 0 && (index < 126 || c == '\n')) {		// if there is a character and index isnt out of bounds or character is a newline
			if (c < 0x80 && c >= 0x20) {				// if character is printable
				text[0] = c;							// this is to make sure kprint can print the character
				text[1] = '\0';
				kprint(text);
				command[index++] = c;					// add the character to the command string
			} else if (c == '\n') {						// if character is a newline
				kprint("\n");							// put a newline
				command[index++] = '\0';				// put a null terminator on the end of command
				// ===== COMMAND SELECTOR =====
				if (kstrcmp("exit", command) == true) {
					return;
				} else if (kstrcmp("cls", command) == true) {
					kclear_vga_buffer();
					kprint("\n");
				} else if (kstrcmp("uname", command) == true) {
					kprint("AKOS(C) Kernel v0.1-3.2\n");
				} else if (kstrcmp("help", command) == true) {
					for (int i = 0; i < 6; i++) kprint(help_message[i]);
				} else if (kstrcmp("reboot", command) == true) {
					reboot(0x83da89ff341ace34ULL);
				} else if (kstrcmp("pong", command)) {
					pong();
				} else {
					kprint("Invalid command\n");
				}

				kmem_zero((address)command, (address)command + index);		// zero the command
				index = 0;						// put index to first character
				kprint("> ");					// print prompt
			} else if (c == '\b' && index > 0) {	// character is a backspace and index isnt less than 1 to prevent underflow
				command[--index] = '\0';			// decrement first and put a \0 instead of the last letter
				kprint("\r> ");					// mov the cursor to the start of the line and print prompt
				kprint(command);					// print command
				kprint(" \b");					// idk why this, i dont care tho bc it works - new me: the character is still left
																	// in the vga buffer so we need to overwrite it and move the cursor back
			}
		}
	}
}