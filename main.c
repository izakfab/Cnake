#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "terminal.h"

int main() {

	terminal_init();
	clear();
	setbuf(stdout, NULL);
    int width = 40;
	int height = 20;

	draw_box(width, height, 10, 5);
	int x = 1;
	int y = 2;

	int sw = 0;
	int running = 1;
	int direction = 0; // 0: up 1: down 2: right 3: left
	while (running == 1) {
		while (!kbhit()) {
			move_cursor(12 + x * 2, 7 + y);
			printf("  ");
			if (direction == 0) y = y == 0 ? y : y - 1;
			else if (direction == 1) y = y == height - 1 ? y : y + 1;
			else if (direction == 2) x = x * 2 == width - 2 ? x : x + 1;
			else x = x == 0 ? x : x - 1;
			move_cursor(12 + x * 2, 7 + y);
			if (sw == 0) {
				print_green("██");
				sw = 1;
			} else {
				print_red("██");
				sw = 0;
			}
		}
		char ch;
		read(STDIN_FILENO, &ch, 1);
		if (ch == 27) {
			if (!kbhit())
				running = 0;
			else {
				read(STDIN_FILENO, &ch, 1);
				if (ch == 91) {
					read(STDIN_FILENO, &ch, 1);
					direction = ch - 'A';
				}
			}
		}
		move_cursor(12 + x * 2, 7 + y);
		printf("  ");
		if (direction == 0) y = y == 0 ? y : y - 1;
		else if (direction == 1) y = y == height - 1 ? y : y + 1;
		else if (direction == 2) x = x * 2 == width - 2 ? x : x + 1;
		else x = x == 0 ? x : x - 1;
		move_cursor(12 + x * 2, 7 + y);
		if (sw == 0) {
			print_green("██");
			sw = 1;
		} else {
			print_red("██");
			sw = 0;
		}
	}
	terminal_cleanup();

    return 0;
}
