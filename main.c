#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "terminal.h"

typedef struct _snake_body {
	int x;
	int y;
	char *oblika;
	struct _snake_body *next;
} snake_body;


void odstrani_zadnjo(snake_body* glava) {
	if (glava -> next -> next == NULL) {
		move_cursor(12 + glava -> next -> x * 2, 7 + glava -> next -> y);
		printf("  ");
		move_cursor(0, 20);
		free(glava -> next);
		glava -> next = NULL;
	}
	else odstrani_zadnjo(glava -> next);
}

snake_body* preveri_self_collision(snake_body* glava, int x, int y) {
	if (glava -> next == NULL) return NULL;
	if (glava -> x == x && glava -> y == y) return glava;
	return preveri_self_collision(glava -> next, x, y);
}

snake_body* premik(snake_body* glava, int smer, int width, int height, int* running) {
	snake_body* nova_glava = (snake_body*) malloc(sizeof(snake_body));
	nova_glava -> x = glava -> x;
	nova_glava -> y = glava -> y;
	nova_glava -> next = glava;
	if (smer == 0) {
		if (glava -> y == 0) {
			for (int i = 0; i < 3; i++) {
				move_cursor(12 + glava -> x * 2, 6);
				printf("--");
				usleep(500000);
				move_cursor(12 + glava -> x * 2, 6);
				print_red("██");
				usleep(500000);
			}
			*running = 0;
		} else nova_glava -> y--;
	} else if (smer == 1) {
		if (glava -> y == height - 1) {
			for (int i = 0; i < 3; i++) {
				move_cursor(12 + glava -> x * 2, 7 + height);
				printf("--");
				usleep(500000);
				move_cursor(12 + glava -> x * 2, 7 + height);
				print_red("██");
				usleep(500000);
			}
			*running = 0;
		} else nova_glava -> y++;
	} else if (smer == 2) {
		if (glava -> x * 2 == width - 2) {
			for (int i = 0; i < 3; i++) {
				move_cursor(12 + width, 7 + glava -> y);
				printf("| ");
				usleep(500000);
				move_cursor(12 + width, 7 + glava -> y);
				print_red("██");
				usleep(500000);
			}
			*running = 0;
		} else nova_glava -> x++;
	} else {
		if (glava -> x == 0) {
			for (int i = 0; i < 3; i++) {
				move_cursor(10, 7 + glava -> y);
				printf(" |");
				usleep(500000);
				move_cursor(10, 7 + glava -> y);
				print_red("██");
				usleep(500000);
			}
			*running = 0;
		} else nova_glava -> x--;
	}
	snake_body* potencialni_zadetek = preveri_self_collision(glava, nova_glava -> x, nova_glava -> y);
	if (potencialni_zadetek != NULL) {
		for(int i = 0; i < 3; i++) {
			move_cursor(12 + nova_glava -> x * 2, 7 + nova_glava -> y);
			print_green(potencialni_zadetek -> oblika);
			usleep(500000);
			move_cursor(12 + nova_glava -> x * 2, 7 + nova_glava -> y);
			print_red("██");
			usleep(500000);
		}
		*running = 0;
		return glava;
	}
	return nova_glava;
}

int main() {

	snake_body* glava = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t1 = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t2 = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t3 = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t4 = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t5 = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t6 = (snake_body*) malloc(sizeof(snake_body));
	t6 -> x = 4; t6 -> y = 10; t6 -> next = NULL;
	t5 -> x = 5; t5 -> y = 10; t5 -> next = t6;
	t4 -> x = 6; t4 -> y = 10; t4 -> next = t5;
	t3 -> x = 7; t3 -> y = 10; t3 -> next = t4;
	// t2 -> x = 8; t2 -> y = 10; t2 -> next = NULL;
	t2 -> x = 8; t2 -> y = 10; t2 -> next = t3;
	t1 -> x = 9; t1 -> y = 10; t1 -> next = t2;
	glava -> x = 10; glava -> y = 10; glava -> next = t1;

	terminal_init();
	clear();
	setbuf(stdout, NULL);
    int width = 40;
	int height = 20;

	draw_box(width, height, 10, 5);

	int sw = 0;
	int running = 1;
	int prev_direction = 2;
	int direction = 2; // 0: up 1: down 2: right 3: left
	int timer = 0, trigger = 7;
	int before = clock() * 10000 / CLOCKS_PER_SEC;
	int changed = 0; // Prevents multiple direction changes per tick
	while (running == 1) {
		if (kbhit()) {
			char ch;
			read(STDIN_FILENO, &ch, 1);
			if (ch == 27) {
				if (!kbhit())
					running = 0;
				else {
					read(STDIN_FILENO, &ch, 1);
					if (ch == 91 && !changed) {
						read(STDIN_FILENO, &ch, 1);
						if (ch + direction != 'B' && ch + direction != 'F') { // Checks that the directions are not opposite 'B' = 'B' + 0 = 'A' + 1 and 'F' = 'D' + 2 = 'c' + 3
							direction = ch - 'A';
							changed = 1;
						}
					}
				}
			}
		}
		clock_t difference = clock();
		timer = difference * 10000 / CLOCKS_PER_SEC;
		if (timer - before > trigger) {
			changed = 0;
			snake_body* nova_glava = premik(glava, direction, width, height, &running);
			move_cursor(12 + nova_glava -> x * 2, 7 + nova_glava -> y);
			print_dark_green("██");
			move_cursor(12 + glava -> x * 2, 7 + glava -> y);
			char* oblika;
			if ((direction == 0 || direction == 1) && (prev_direction == 0 || prev_direction == 1)) oblika = "▐▌";
			else if ((direction == 2 || direction == 3) && (prev_direction == 2 || prev_direction == 3)) oblika = "■■";
			else if ((direction == 2 && prev_direction == 0) || (direction == 1 && prev_direction == 3)) oblika = " ▄";
			else if ((direction == 1 && prev_direction == 2) || (direction == 3 && prev_direction == 0)) oblika = "▄ ";
			else if ((direction == 3 && prev_direction == 1) || (direction == 0 && prev_direction == 2)) oblika = "▀ ";
			else if ((direction == 0 && prev_direction == 3) || (direction == 2 && prev_direction == 1)) oblika = " ▀";
			print_green(oblika);
			glava -> oblika = oblika;
			odstrani_zadnjo(nova_glava);
			before = clock() * 10000 / CLOCKS_PER_SEC;
			glava = nova_glava;
			prev_direction = direction;
		}
	}
	move_cursor(12 + width, 7 + height);
	printf("\n");
	terminal_cleanup();

    return 0;
}
