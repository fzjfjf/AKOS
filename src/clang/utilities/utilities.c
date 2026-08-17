#include "../../clangh/utilities/utilities.h"
#include "../../clangh/stdlib/kstdlib.h"

void sl(int argc, char **argv)
{
	kclear_vga_buffer();

	static const char voz[2001] =
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"      ====        ________                ___________                           "
"  _D _|  |_______/        \\__I_I_____===__|_________|                          "
"   |(_)---  |   H\\________/ |   |        =|___ ___|                            "
"   /     |  |   H  |  |     |   |         ||_| |_||                             "
"  |      |  |   H  |__--------------------| [___] |                             "
"  | ________|___H__/__|_____/[][]~\\_______|       |                            "
"  |/ |   |-----------I_____I [][] []  D   |=======|__                           "
"__/ =| o |=-~~\\  /~~\\  /~~\\  /~~\\ ____Pl__|___________                      "
" |/-=|___|=    ||    ||    ||    |_____/~\\___/                                 "
"  \\_/      \\_O=====O=====O=====O_/      \\_/                                  "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                ";

	// address8 vga = (address8)VGA_ADDRESS;
	//
	//
	// for (int i = 0; i < 2000; i++) {
	// 	*vga = voz[i];
	// 	vga++;
	// 	*vga = VGA_WHITE_ON_BLACK;
	// 	vga++;
	// }

	kprint(voz);


	while (1) {		// NOLINT
		__asm__ volatile ("hlt");
	}
}