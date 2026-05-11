#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
	while (running == 1) {
		while (!kbhit()) {
			move_cursor(12 + x * 2, 7 + y);
			if (sw == 0) {
				print_green("██");
				sw = 1;
			} else {
				print_red("██");
				sw = 0;
			}
		}
		move_cursor(12 + x * 2, 7 + y);
		if (sw == 0) {
			print_green("██");
			sw = 1;
		} else {
			print_red("██");
			sw = 0;
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
					move_cursor(12 + x * 2, 7 + y);
					printf("  ");
					if (ch == 'A' && y != 0) y--;
					else if (ch == 'B' && y != height - 1) y++;
					else if (ch == 'C' && x * 2 != width - 2) x++;
					else if (ch == 'D' && x != 0) x--;
					move_cursor(12 + x * 2, 7 + y);
					if (sw == 0) {
						print_green("██");
					} else {
						print_red("██");
					}
				}
			}

		}
	}
	terminal_cleanup();

    return 0;
}
