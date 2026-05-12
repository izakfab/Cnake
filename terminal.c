#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include "terminal.h"

static struct termios old_attr;

void terminal_init() {
	struct termios new_attr;
	tcgetattr(STDIN_FILENO, &old_attr);
	new_attr = old_attr;
	new_attr.c_lflag &= ~(ICANON | ECHO);
	new_attr.c_cc[VMIN] = 1;
	new_attr.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
	printf("%c[?25l", ASCII_ESC);
}

void terminal_cleanup() {
	tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
	printf("%c[?25h", ASCII_ESC);
}

int kbhit() {
	fd_set set;
	struct timeval tv;
	FD_ZERO(&set);
	FD_SET(STDIN_FILENO, &set);
	tv.tv_sec = 0;
	tv.tv_usec = 10000;
	return select(STDIN_FILENO + 1, &set, NULL, NULL, &tv);
}


void clear() {
	printf("%c[2J", ASCII_ESC);
}

void move_cursor(int x, int y) {
	printf("%c[%d;%dH", ASCII_ESC, y, x);
}

void draw_box(int width, int height, int offx, int offy) {
	move_cursor(1 + offx, 1 + offy);
	printf("╔");
	for (int i = 0; i < width; i++) printf("═");
	printf("╗");
	for (int i = 0; i < height; i++) {
		move_cursor(1 + offx, i + 2 + offy);
		printf("║");
		move_cursor(width + 2 + offx, i + 2 + offy);
		printf("║");
	}
	move_cursor(1 + offx, height + 2 + offy);
	printf("╚");
	for (int i = 0; i < width; i++) printf("═");
	printf("╝");
}

void print_red(char text[]) {
	printf("%c[38;5;196m%s%c[0m", ASCII_ESC, text, ASCII_ESC);
}
void print_green(char text[]) {
	printf("%c[38;5;34m%s%c[0m", ASCII_ESC, text, ASCII_ESC);
}
void print_dark_green(char text[]) {
	printf("%c[38;5;22m%s%c[0m", ASCII_ESC, text, ASCII_ESC);
}
