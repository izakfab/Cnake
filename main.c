#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "terminal.h"

int main() {

	terminal_init();
	clear();
	setbuf(stdout, NULL);
    int width = 40;
	int height = 20;

	draw_box(width, height, 10, 5);
	int x = 10;
	int y = 10;

	int sw = 0;
	int running = 1;
	int direction = 0; // 0: up 1: down 2: right 3: left
	int timer = 0, trigger = 5;
	int before = clock() * 10000 / CLOCKS_PER_SEC;
	while (running == 1) {
		if (kbhit()) {
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
		}
		clock_t difference = clock();
		timer = difference * 10000 / CLOCKS_PER_SEC;
		// printf("%d %d %d\n%d", timer, before, trigger, timer - before);
		if (timer - before > trigger) {
			move_cursor(12 + x * 2, 7 + y);
			printf("  ");
			if (direction == 0) {
				if (y == 0) {
					move_cursor(12 + x * 2, 6);
					print_red("██");
					running = 0;
				} else y--;
			} else if (direction == 1) {
				if (y == height - 1) {
					move_cursor(12 + x * 2, 7 + height);
					print_red("██");
					running = 0;
				} else y++;
			} else if (direction == 2) {
				if (x * 2 == width - 2) {
					move_cursor(12 + width, 7 + y);
					print_red("██");
					running = 0;
				} else x++;
			} else {
				if (x == 0) {
					move_cursor(10, 7 + y);
					print_red("██");
					running = 0;
				} else x--;
			}
			move_cursor(12 + x * 2, 7 + y);
			print_dark_green("██");
			before = clock() * 10000 / CLOCKS_PER_SEC;
		}
	}
	move_cursor(12 + width, 7 + height);
	printf("\n");
	terminal_cleanup();

    return 0;
}
