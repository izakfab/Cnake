#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "terminal.h"

typedef struct _snake_body {
	int x;
	int y;
	char *oblika;
	struct _snake_body *next;
} snake_body;

typedef struct _apple {
	int x;
	int y;
} apple;


void odstrani_zadnjo(snake_body* glava, int offsetx, int offsety) {
	if (glava -> next -> next == NULL) {
		move_cursor(offsetx + glava -> next -> x * 2, offsety + glava -> next -> y);
		printf("  ");
		move_cursor(0, 20);
		free(glava -> next);
		glava -> next = NULL;
	}
	else odstrani_zadnjo(glava -> next, offsetx, offsety);
}

int prosto(snake_body* glava, int x, int y) {
	if (glava == NULL) return 1;
	if (glava -> x == x && glava -> y == y) return 0;
	return prosto(glava -> next, x, y);
}

apple* ustvari_jabolko(snake_body* glava, int width, int height, int offsetx, int offsety) {
	apple* jabolko = (apple*) malloc(sizeof(apple));
	int x = 0;
	int y = 0;
	do {
		x = rand() % width;
		y = rand() % height;
	} while (!prosto(glava, x, y));
	move_cursor(offsetx + x * 2, offsety + y);
	print_red("◖◗");
	jabolko -> x = x;
	jabolko -> y = y;
	return jabolko;
}

snake_body* preveri_self_collision(snake_body* glava, int x, int y) {
	if (glava -> next == NULL) return NULL;
	if (glava -> x == x && glava -> y == y) return glava;
	return preveri_self_collision(glava -> next, x, y);
}

snake_body* premik(snake_body* glava, int smer, int width, int height, int* running, int offsetx, int offsety) {
	snake_body* nova_glava = (snake_body*) malloc(sizeof(snake_body));
	nova_glava -> x = glava -> x;
	nova_glava -> y = glava -> y;
	nova_glava -> next = glava;
	if (smer == 0) {
		if (glava -> y == 0) {
			for (int i = 0; i < 3; i++) {
				move_cursor(offsetx + glava -> x * 2, offsety - 1);
				printf("══");
				usleep(500000);
				move_cursor(offsetx + glava -> x * 2, offsety - 1);
				print_red("██");
				usleep(500000);
			}
			*running = 0;
		} else nova_glava -> y--;
	} else if (smer == 1) {
		if (glava -> y == height - 1) {
			for (int i = 0; i < 3; i++) {
				move_cursor(offsetx + glava -> x * 2, offsety + height);
				printf("══");
				usleep(500000);
				move_cursor(offsetx + glava -> x * 2, offsety + height);
				print_red("██");
				usleep(500000);
			}
			*running = 0;
		} else nova_glava -> y++;
	} else if (smer == 2) {
		if (glava -> x == width - 1) {
			for (int i = 0; i < 3; i++) {
				move_cursor(offsetx + width * 2, offsety + glava -> y);
				printf("║ ");
				usleep(500000);
				move_cursor(offsetx + width * 2, offsety + glava -> y);
				print_red("██");
				usleep(500000);
			}
			*running = 0;
		} else nova_glava -> x++;
	} else {
		if (glava -> x == 0) {
			for (int i = 0; i < 3; i++) {
				move_cursor(offsetx - 2, offsety + glava -> y);
				printf(" ║");
				usleep(500000);
				move_cursor(offsetx - 2, offsety + glava -> y);
				print_red("██");
				usleep(500000);
			}
			*running = 0;
		} else nova_glava -> x--;
	}
	snake_body* potencialni_zadetek = preveri_self_collision(glava, nova_glava -> x, nova_glava -> y);
	if (potencialni_zadetek != NULL && *running == 1) {
		for(int i = 0; i < 3; i++) {
			move_cursor(offsetx + nova_glava -> x * 2, offsety + nova_glava -> y);
			print_green(potencialni_zadetek -> oblika);
			usleep(500000);
			move_cursor(offsetx + nova_glava -> x * 2, offsety + nova_glava -> y);
			print_red("██");
			usleep(500000);
		}
		*running = 0;
		return glava;
	}
	return nova_glava;
}

int main(int argc, char** argv) {

    int WIDTH = 20;
	int HEIGHT = 20;
	int OFFSETX = 6;
	int OFFSETY = 3;
	int TRIGGER = 500;
	
	// if (argc > 1)

	snake_body* glava = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t1 = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t2 = (snake_body*) malloc(sizeof(snake_body));
	t2 -> x = WIDTH / 2  - 2; t2 -> y = HEIGHT / 2; t2 -> next = NULL;
	t1 -> x = WIDTH / 2 - 1; t1 -> y = HEIGHT / 2; t1 -> next = t2;
	glava -> x = WIDTH / 2; glava -> y = HEIGHT /2; glava -> next = t1;


	terminal_init();
	clear();
	setbuf(stdout, NULL);
	srand(time(NULL));

	draw_box(WIDTH * 2, HEIGHT, OFFSETX, OFFSETY);
	apple* jabolko = ustvari_jabolko(glava, WIDTH, HEIGHT, OFFSETX + 2, OFFSETY + 2);

	int sw = 0;
	int running = 1;
	int prev_direction = 2;
	int direction = 2; // 0: up 1: down 2: right 3: left
	struct timeval tv_before;
	gettimeofday(&tv_before, NULL);
	int changed = 0; // Prevents multiple direction changes per tick
	int score = 3;
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
		struct timeval tv_now;
		gettimeofday(&tv_now, NULL);
		int elapsed_ms = (tv_now.tv_sec - tv_before.tv_sec) * 1000
		               + (tv_now.tv_usec - tv_before.tv_usec) / 1000;
		if (elapsed_ms > TRIGGER) {
			changed = 0;
			snake_body* nova_glava = premik(glava, direction, WIDTH, HEIGHT, &running, OFFSETX + 2, OFFSETY + 2);
			move_cursor(OFFSETX + 2 + nova_glava -> x * 2, OFFSETY + 2 + nova_glava -> y);
			print_dark_green("██");
			char* oblika;
			if ((direction == 0 || direction == 1) && (prev_direction == 0 || prev_direction == 1)) oblika = "▐▌";
			else if ((direction == 2 || direction == 3) && (prev_direction == 2 || prev_direction == 3)) oblika = "■■";
			else if ((direction == 2 && prev_direction == 0) || (direction == 1 && prev_direction == 3)) oblika = " ▄";
			else if ((direction == 1 && prev_direction == 2) || (direction == 3 && prev_direction == 0)) oblika = "▄ ";
			else if ((direction == 3 && prev_direction == 1) || (direction == 0 && prev_direction == 2)) oblika = "▀ ";
			else if ((direction == 0 && prev_direction == 3) || (direction == 2 && prev_direction == 1)) oblika = " ▀";
			glava -> oblika = oblika;
			if (nova_glava -> x == jabolko -> x && nova_glava -> y == jabolko -> y) {
				free(jabolko);
				score++;
				if (score == WIDTH * HEIGHT) (running = 0);
				else jabolko = ustvari_jabolko(nova_glava, WIDTH, HEIGHT, OFFSETX + 2, OFFSETY + 2);
			} else odstrani_zadnjo(nova_glava, OFFSETX + 2, OFFSETY + 2);
			move_cursor(OFFSETX + 2 + glava -> x * 2, OFFSETY + 2 + glava -> y);
			print_green(oblika);
			if (running) {
				move_cursor(OFFSETX + 2 + nova_glava -> x * 2, OFFSETY + 2 + nova_glava -> y);
				print_dark_green("██");
			}
			gettimeofday(&tv_before, NULL);
			glava = nova_glava;
			prev_direction = direction;
		}
	}
	move_cursor(OFFSETX + 2 + WIDTH * 2, OFFSETY + 2 + HEIGHT);
	printf("\n");
	terminal_cleanup();

    return 0;
}
