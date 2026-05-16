#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include "terminal.h"
#include "Cnake.h"

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

void draw_snake(snake_body* head, int offx, int offy, int first) {
	if (head == NULL) return;
	move_cursor(offx + head -> x * 2 + 3, offy + head -> y + 2);
	if (first) print_dark_green("██");
	else print_green(head -> oblika);
	draw_snake(head -> next, offx, offy, 0);
}

void draw_start_screen(int width, int height, int offx, int offy) {
	draw_box(width * 2, height, offx + 1, offy);
	move_cursor(offx + (width / 2 - 10) * 2 - 1, offy + 2);
	printf("MM'\"\"\"\"'YMM                   dP");
	move_cursor(offx + (width / 2 - 10) * 2 - 1, offy + 3);
	printf("M' .mmm. `M                   88\n");
	move_cursor(offx + (width / 2 - 10) * 2 - 1, offy + 4);
	printf("M  MMMMMooM 88d888b. .d8888b. 88  .dP  .d8888b.");
	move_cursor(offx + (width / 2 - 10) * 2 - 1, offy + 5);
	printf("M  MMMMMMMM 88'  `88 88'  `88 88888\"   88ooood8");
	move_cursor(offx + (width / 2 - 10) * 2 - 1, offy + 6);
	printf("M. `MMM' .M 88    88 88.  .88 88  `8b. 88.  ...");
	move_cursor(offx + (width / 2 - 10) * 2 - 1, offy + 7);
	printf("MM.     .dM dP    dP `88888P8 dP   `YP `88888P'");
	move_cursor(offx + (width / 2 - 10) * 2 - 1, offy + 8);
	printf("MMMMMMMMMMM");
	move_cursor(offx + (width / 2 - 6) * 2 - 1, offy + 10);
	printf("Press any key to start the game.");
}

void draw_apple(apple* apple, int offx, int offy) {
	move_cursor(offx + apple -> x * 2 + 3, offy + apple -> y + 2);
	print_red("◖◗");
}

void draw_button(char* text, int offx, int offy) {
	move_cursor(1 + offx, 1 + offy);

	printf("┌");
	int length = 0;
	for (char* c = text; *c != 0; c++) {
		length++;
		printf("─");
	}
	printf("┐");
	move_cursor(1 + offx, 2 + offy);
	printf("│%s│", text);
	move_cursor(1 + offx, 3 + offy);
	printf("└");
	for (int i = 0; i < length; i++) printf("─");
	printf("┘");
}
void select_button(char* text, int offx, int offy) {
	printf("%c[7;1m", ASCII_ESC);
	draw_button(text, offx, offy);
	printf("%c[0m", ASCII_ESC);
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
