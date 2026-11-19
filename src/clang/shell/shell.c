#include <shell.h>
#include <keyboard.h>
#include <kstdlib.h>
#include <vector.h>

extern void kpanic();

typedef char* CPTR;
DEFINE_VECTOR(CPTR, StrVec)

void dispatcher(char *command, int index, char *help_message[],				// for built in commands
				program_t programs[], char *names[], size_t count)			// for programs passed from kernel
{
	// TODO: add the programs[] support

	if (index == 0 || *command == ' ' || *command == '\0') {
		return;
	}

	if (kstrcmp_ws(command, "cls") == true) {
		kclear_vga_buffer();
		kprint("\n");
	} else if (kstrcmp_ws(command, "uname") == true) {
		kprint("AKOS(C) Kernel v0.1-4\n");
	} else if (kstrcmp_ws(command, "help") == true) {
		for (int i = 0; help_message[i]; i++) {
			kprint(help_message[i]);
		}
		kprint("\nOther commands or programs:\n");
		for (int i = 0; i < count; i++) {
			kprint(names[i]);
			kprint("\n");
		}

	} else if (kstrcmp_ws(command, "reboot") == true) {
		reboot(0x83da89ff341ace34ULL);
	} else if (kstrcmp_ws(command, "debugpanic") == true) {
		kpanic();
	} else if (kstrcmp_ws(command, "debugtestgpf") == true) {
		__asm__ volatile("int $13");
	} else if (kstrcmp_ws(command, "debugcorruptstack") == true) {
		for (int i = 0; i < 100; i++) __asm__ volatile("pop %eax");
		for (int i = 0; i < 100; i++) __asm__ volatile("push $0xffffffff");
	} else if (kstrcmp_ws(command, "debugcorruptstackandpanic") == true) {
		for (int i = 0; i < 100; i++) __asm__ volatile("pop %eax");
		for (int i = 0; i < 100; i++) __asm__ volatile("push $0xffffffff");
		kpanic();
	} else if (index == 1) {

	} else {
		// first go through the program function pointer array
		for (int i = 0; i < count; i++) {
			if (kstrcmp_ws(command, names[i]) == true) {
				StrVec argv;									//make a vector for ourselves
				StrVec_init(&argv);

				char *p = command;								// to not overwrite command

				while (*p) {									// while we have not reached end
					while (*p == ' ') *p++ = '\0';				// skip spaces and repalce them with NULL
					if (*p == '\0') break;						// if we reached end
					if (!StrVec_push(&argv, p)) 				/* push the pointer */
						goto error;
					while (*p && *p != ' ') p++;				// skip word (token)
				}

				if (!StrVec_push(&argv, NULL)) goto error;				// stavljamo NULL na kraj

				programs[i]((int)argv.size - 1, argv.data);				// - 1 zbog NULL na kraju

				StrVec_free(&argv);
				return;


				error:
				// =============================================================================================
				kprintf("MEMORY FAULT: ARGUMENT PARSING FAILED\n");
				kprintf("CONTINUE TO SHELL OR PANIC? (c or p): ");

				char c;
				label:
				do {
					c = kgetchar_nb();
					if (!c) __asm__ volatile("hlt");
				} while (!c);

				if (c == 'p') {
					kpanic();
				} else if (c == 'c') {
					kprintf("\n");
					StrVec_free(&argv);
					return;
				} else {
					goto label;
				}
				// =============================================================================================
			}
		}

		// didnt match any programs, now say invalid command
		kprint("Invalid command!\n");
	}
}

void shell(program_t programs[], char *names[], size_t count)
{
	char command[128] = {0};
	unsigned char c = ' ';
	int index = 0;
	char text[2] = {0};

	char *help_message[] = {
		"CLS - Clear the screen\n",
		"UNAME - Show version number\n",
		"HELP - Show help message\n",
		"REBOOT - Reboot PC\n",
		NULL
	};

	kprint("> ");

	while (1) {											// NOLINT - currently not needed to stop
		c = kgetchar_nb();					// get input
		if (c != 0 && (index < 126 || c == '\n')) {		// if there is a character and index isnt out of bounds or character is a newline
			if (c < 0x80 && c >= 0x20) {				// if character is printable
				text[0] = c;							// this is to make sure kprint can print the character
				text[1] = '\0';
				kprint(text);
				command[index++] = c;					// add the character to the command string
			} else if (c == '\n') {						// if character is a newline
				kprint("\n");							// put a newline
				command[index++] = ' ';				// put a null terminator on the end of command
				command[index++] = '\0';

				dispatcher(command, index, help_message, programs, names, count);

				kmem_zero((address8)command, (address8)command + index);		// zero the command
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

		__asm__ volatile("hlt");
	}
}