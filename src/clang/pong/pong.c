// ======== INCLUDES ========
#include <pong.h>
#include <kstdlib.h>
#include <keyboard.h>

// ======== DEFINES AND ENUMS ========
#define MAX_Y 24
extern uint8_t inb(uint16_t port);
enum {
	UP, DOWN
};
enum {
	HIT_LEFT_PADDLE, HIT_RIGHT_PADDLE, NO_HIT, MISSED_PADDLE, HIT_CEIL_OR_FLOOR, CORNER_EDGE_CASE, OUT_OF_BOUNDS
};

// ======== VARIABLE DEFINITIONS ========
typedef struct {
	int color;
	int len;
	int y;
	const int x;
} PONG_paddle_t;

typedef struct {
	int x;
	int y;
	int color;
	bool moving_right;
	bool moving_up;
} PONG_ball_t;

typedef struct {
	int right;
	int left;
} PONG_scores_t;

// ======== VARIABLE DECLARATIONS ========
VGA_t vga_pong_args = {
	.vga = (address)VGA_ADDRESS,
	.line_number = 0,
	.column_number = 0,
	.color = VGA_BLACK_ON_BLACK,
	.remove_line_below = true,
};

PONG_paddle_t left_paddle = {
	.color = VGA_WHITE_ON_BLACK,
	.len = 5,
	.y = 9,
	.x = 2
};

PONG_paddle_t right_paddle = {
	.color = VGA_WHITE_ON_BLACK,
	.len = 5,
	.y = 9,
	.x = 77,
};

PONG_ball_t ball = {
	.color = VGA_WHITE_ON_BLACK,
	.x = 40,
	.y = 9,
	.moving_right = true,
	.moving_up = true,
};

PONG_scores_t score = {
	.right = 0,
	.left = 0,
};

// ======== FUNCTION DECLARATIONS ========
void reset_to_default()
{
	vga_pong_args.line_number = 0;
	vga_pong_args.column_number = 0;
	vga_pong_args.remove_line_below = true;
	vga_pong_args.vga = (address)VGA_ADDRESS;

	ball.x = 40;
	ball.y = 9;
	ball.moving_up = true;
	ball.moving_right = true;

	left_paddle.y = 9;
	right_paddle.y = 9;
}

void draw_char(int x, int y, uchar c, int color)
{
	address vgap = (address)(VGA_ADDRESS + (y * 80 + x) * 2);
	*vgap = c;
	*(vgap + 1) = color;
}

void draw_paddle(int x, int y, int color)
{
	for (int i = 0; i < 5; i++) {
		if (y + i> 24) continue;
		draw_char(x, y + i, 0xDB, color);
	}
}

void move_paddle(PONG_paddle_t *paddle, int direction)
{
	if (paddle->y >= 0) {
		if (direction == UP && paddle->y > 0) {
			paddle->y--;
			draw_paddle(paddle->x, paddle->y, paddle->color);
			draw_char(paddle->x, paddle->y + 5, ' ', VGA_BLACK_ON_BLACK);
		} else if (direction == DOWN && paddle->y < MAX_Y - 4) {
			paddle->y++;
			draw_paddle(paddle->x, paddle->y, paddle->color);
			draw_char(paddle->x, paddle->y - 1, ' ', VGA_BLACK_ON_BLACK);
		}
	}
}

int check_collision()
{
	int return_value = NO_HIT;		// store the return value here
	// y values
	int paddle_y = 0;
	int ball_y = ball.y;

	// check if out of bounds
	if (ball.y > 24) {
		return OUT_OF_BOUNDS;
	}

	// if ball is in these x, y positions (edge cases):
	// (3, 0); (3, 24); (77, 0); (77, 24)
	if ((ball.x == 3 && ball.y == 0) ||
		(ball.x == 3 && ball.y == 24) ||
		(ball.x == 76 && ball.y == 0) ||
		(ball.x == 76 && ball.y == 24)) {
		return CORNER_EDGE_CASE;
	}

	// if ball is on top or bottom
	if (ball_y == 0 || ball_y == 24) {
		return_value = HIT_CEIL_OR_FLOOR;
		goto exit;
	}
	// if ball is behind paddle, put that the paddle is missed and return immediately
	if (ball.x < 2 || ball.x > 77) {
		return_value = MISSED_PADDLE;
		goto exit;
	}

	// if ball is in front of paddle, go to corresponding paddle
	if (ball.x == 3) goto left;
	if (ball.x == 76) goto right;
	// else exit and say there is no collision
	else {
		return_value = NO_HIT;
		goto exit;
	}

	// check for left paddle
	left:
	paddle_y = left_paddle.y;
	for (int i = 0; i < 5; i++) {
		if (paddle_y + i == ball_y) return_value = HIT_LEFT_PADDLE;
	}
	goto exit;		// prevent fall through

	// check for right paddle
	right:
	paddle_y = right_paddle.y;
	for (int i = 0; i < 5; i++) {
		if (paddle_y + i == ball_y) return_value = HIT_RIGHT_PADDLE;
	}
	goto exit;		// redundant but here for consistency, NOLINT to shut linter up

	exit:
	return return_value;
}

int move_ball()
{
	draw_char(ball.x, ball.y, ' ', ball.color);

	if (ball.moving_right) ball.x++;
	if (!ball.moving_right) ball.x--;
	if (ball.moving_up) ball.y--;
	if (!ball.moving_up) ball.y++;

	draw_char(ball.x, ball.y, 0xdb, ball.color);
}

// ======== MAIN FUNCTION ========
void pong()
{
	// TODO: rewrite from scratch. this is stupid. future me: rewriting currently
	kclear_vga_buffer();

	bool ai = false;

	kprint("AI or 2-player mode?\n> ");
	char ans[128] = "";
	kinput_b(ans);
	if (kstrcmp(ans, "ai\n")) ai = true;

	kclear_vga_buffer();

	draw_paddle(right_paddle.x, right_paddle.y, right_paddle.color);
	draw_paddle(left_paddle.x, left_paddle.y, left_paddle.color);

	draw_char(0, 0, '0', VGA_WHITE_ON_BLACK);
	draw_char(79, 0, '0', VGA_WHITE_ON_BLACK);
	int tick = 0;
	int hit_pos = 0;

	while (1) {		// NOLINT
		// ======== GAME LOOP ========
		char c = kgetchar_nb();
		if (c == 'w' && !ai) {
			move_paddle(&left_paddle, UP);
		} else if (c == 's' && !ai) {
			move_paddle(&left_paddle, DOWN);
		} else if (c == 'i') {
			move_paddle(&right_paddle, UP);
		} else if (c == 'k') {
			move_paddle(&right_paddle, DOWN);
		} else if (c == 'q') {
			kclear_vga_buffer();
			return;
		}

		if (tick == 500000) {
			move_ball();
			tick = 0;

			if (ai) {
				while (ball.y != left_paddle.y) {
					if (ball.y > left_paddle.y) {
						move_paddle(&left_paddle, DOWN);
					} else {
						move_paddle(&left_paddle, UP);
					}

					if (left_paddle.y > 19) break;
				}
			}

			int rvalue = check_collision();

			switch (rvalue) {
				case NO_HIT:
					break;
				case HIT_LEFT_PADDLE:
					hit_pos = ball.y - left_paddle.y;
					if (hit_pos <= 1) {
						ball.moving_up = true;
					} else if (hit_pos == 2) {
						ball.moving_up = !ball.moving_up;
					} else {
						ball.moving_up = false;
					}
					ball.moving_right = true;
					break;
				case HIT_RIGHT_PADDLE:
					hit_pos = ball.y - right_paddle.y;
					if (hit_pos <= 1) {
						ball.moving_up = true;
					} else if (hit_pos == 2) {
						ball.moving_up = !ball.moving_up;
					} else {
						ball.moving_up = false;
					}
					ball.moving_right = false;
					break;
				case MISSED_PADDLE:
					if (ball.x < 10) {
						// left side lost
						score.right++;
					} else {
						score.left++;
					}

					draw_char(ball.x, ball.y, ' ', ball.color);
					draw_char(0, 0, score.left + 0x30, VGA_WHITE_ON_BLACK);
					draw_char(79, 0, score.right + 0x30, VGA_WHITE_ON_BLACK);

					reset_to_default();
					break;
				case HIT_CEIL_OR_FLOOR:
					ball.moving_up = !ball.moving_up;
					break;
				case CORNER_EDGE_CASE:
					ball.moving_up = !ball.moving_up;
					ball.moving_right = !ball.moving_right;
					break;
				case OUT_OF_BOUNDS:
					kprint("OUT OF BOUNDS, TO BE FIXED; HARD REBOOT PC NOW");
					while (1) {
						__asm__ volatile ("hlt");
					}
					break;
			}
			if (score.right == 10) {
				kclear_vga_buffer();
				kprint("RIGHT WON\n!");
				for (int i = 0; i < 10000000; i++) {
					kdo_nothing();
				}
				return;
			} else if (score.left == 10) {
				kclear_vga_buffer();
				kprint("LEFT WON!\n");
				for (int i = 0; i < 10000000; i++) {
					kdo_nothing();
				}
				return;
			}
		}

		tick++;
	}
}