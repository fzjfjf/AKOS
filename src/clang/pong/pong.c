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
	HIT_LEFT_PADDLE, HIT_RIGHT_PADDLE, NO_HIT, MISSED_PADDLE, HIT_CEIL_OR_FLOOR
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

	// if ball is on top or bottom
	if ((ball_y == 0 || ball_y == 24) && (ball.x != 3 && ball.x != 76)) {
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

	draw_paddle(right_paddle.x, right_paddle.y, right_paddle.color);
	draw_paddle(left_paddle.x, left_paddle.y, left_paddle.color);

	int tick = 0;
	int hit_pos = 0;

	while (1) {		// NOLINT
		// ======== GAME LOOP ========
		char c = kgetchar_nb();
		if (c == 'w') {
			move_paddle(&left_paddle, UP);
		} else if (c == 's') {
			move_paddle(&left_paddle, DOWN);
		} else if (c == 'i') {
			move_paddle(&right_paddle, UP);
		} else if (c == 'k') {
			move_paddle(&right_paddle, DOWN);
		}

		if (tick == 1000000) {
			move_ball();
			tick = 0;

			int rvalue = check_collision();
			if (rvalue == NO_HIT) goto end;
			if (rvalue == HIT_LEFT_PADDLE) goto change_ball_direction_to_right;
			if (rvalue == HIT_RIGHT_PADDLE) goto change_ball_direction_to_left;
			if (rvalue == MISSED_PADDLE) goto missed_paddle;
			if (rvalue == HIT_CEIL_OR_FLOOR) goto ceil_floor;

			ceil_floor:
			ball.moving_up = !ball.moving_up;
			goto end;

			change_ball_direction_to_right:
			hit_pos = ball.y - right_paddle.y;
			if (hit_pos <= 1) {
				ball.moving_up = true;
			} else if (hit_pos == 2) {
				ball.moving_up = !ball.moving_up;
			} else {
				ball.moving_up = false;
			}
			ball.moving_right = true;
			goto end;

			change_ball_direction_to_left:
			hit_pos = ball.y - left_paddle.y;
			if (hit_pos <= 1) {
				ball.moving_up = true;
			} else if (hit_pos == 2) {
				ball.moving_up = !ball.moving_up;
			} else {
				ball.moving_up = false;
			}
			ball.moving_right = false;
			goto end;

			missed_paddle:
			break;
		}

		end:
		tick++;
	}

	kclear_vga_buffer();
	kprint("END");
	for (int i = 0; i < 200000000; i++) {
		kdo_nothing();
	}

	kclear_vga_buffer();
	return;		// NOLINT
}