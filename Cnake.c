#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
	int HEIGHT = 20;
    int WIDTH = 20;
	int TICK_SPEED = 500;
	int OFFSETX = 12;
	int OFFSETY = 6;

	for (int i = 1; i < argc; i++) {
		char* endptr;
		if (!strcmp(argv[i], "--height") || !strcmp(argv[i], "-h")) {
			if (i != argc - 1) {
				int height = strtol(argv[i + 1], &endptr, 10);
				if (*endptr != 0) {
					printf("Wrong type of parameter for flag %s: %s\n%s only accepts integers!\n", argv[i], argv[i + 1], argv[i]);
					return 1;
				} else if (height < 3 || height > INT32_MAX) {
					printf("Height must be at least 3 and less than %d!\n", INT32_MAX);
					return 1;
				} else {
					HEIGHT = height;
					i++;
				}
			} else {
				printf("Missing argument for flag %s! (use --help for list of arguments)\n", argv[i]);
				return 1;
			}
		} else if (!strcmp(argv[i], "--width") || !strcmp(argv[i], "-w")) {
			if (i != argc - 1) {
				int width = strtol(argv[i + 1], &endptr, 10);
				if (*endptr != 0) {
					printf("Wrong type of parameter for flag %s: %s\n%s only accepts integers!\n", argv[i], argv[i + 1], argv[i]);
					return 1;
				} else if (width < 5 || width > INT32_MAX) {
					printf("Width must be at least 5 and less than %d!\n", INT32_MAX);
					return 1;
				} else {
					WIDTH = width;
					i++;
				}
			} else {
				printf("Missing argument for flag %s!\n", argv[i]);
				return 1;
			}
		} else if (!strcmp(argv[i], "--tickspeed") || !strcmp(argv[i], "-t")) {
			if (i != argc - 1) {
				int tick_speed = strtol(argv[i + 1], &endptr, 10);
				if (*endptr != 0) {
					printf("Wrong type of parameter for flag %s: %s\n%s only accepts integers!\n", argv[i], argv[i + 1], argv[i]);
					return 1;
				} else if (tick_speed < 100 || tick_speed > INT32_MAX) {
					printf("Tick speed must be at least 100 and less than %d!\n", INT32_MAX);
					return 1;
				} else {
					TICK_SPEED = tick_speed;
					i++;
				}
			} else {
				printf("Missing argument for flag %s!\n", argv[i]);
				return 1;
			}
		/*} else if (!strcmp(argv[i], "--offsetx")) {
			if (i != argc - 1) {
				int offsetx = strtol(argv[i + 1], &endptr, 10);
				if (*endptr != 0) {
					printf("Wrong type of parameter for flag %s: %s\n%s only accepts integers!\n", argv[i], argv[i + 1], argv[i]);
					return 1;
				} else if (offsetx < 0 || offsetx > INT32_MAX) {
					printf("X offset must be at least 0 and less than %d!\n", INT32_MAX);
					return 1;
				} else {
					OFFSETX = offsetx;
					i++;
				}
			} else {
				printf("Missing argument for flag %s!\n", argv[i]);
				return 1;
			}
		} else if (!strcmp(argv[i], "--offsety")) {
			if (i != argc - 1) {
				int offsety = strtol(argv[i + 1], &endptr, 10);
				if (*endptr != 0) {
					printf("Wrong type of parameter for flag %s: %s\n%s only accepts integers!\n", argv[i], argv[i + 1], argv[i]);
					return 1;
				} else if (offsety < 0 || offsety > INT32_MAX) {
					printf("Y offset must be at least 0 and less than %d!\n", INT32_MAX);
					return 1;
				} else {
					OFFSETY = offsety;
					i++;
				}
			} else {
				printf("Missing argument for flag %s!\n", argv[i]);
				return 1;
			} */
		} else {
			int exit = 0;
			if (strcmp(argv[i], "--help")) {
				printf("%s is not a valid argument!", argv[i]);
				exit = 1;
			} else printf("Cnake is a remake of a Snake game in language C.");
			printf(" Below are usage and the possible command line arguments:\
					\nUsage: Cnake [options]\
					\n\
					\n  -h HEIGHT, --height HEIGHT\
					\n\tSets the height of the playing field to HEIGHT characters (default: 20)\
					\n  -w WIDTH, --width WIDTH\
					\n\tSets the width of the playing field to WIDTH * 2 characters\
					\n\tWIDTH * 2 is used for compatibility with terminals with 2:1 aspect ratio of character height:character width (default: 20)\
					\n  -t TICK_SPEED, --tickspeed TICK_SPEED\
					\n\tSets the length of a game tick to TICK_SPEED in ms (default: 500)\
					\n  --offsetx OFFSET\
					\n\tSets the offset from the left edge to the border to OFFSET characters (default: 12)\
					\n  --offsety OFFSET\
					\n\tSets the offset from the top edge to the border to OFFSET characters (default: 6)\
					\n  --help\
					\n\tPrints this message\n");
			return exit;
		}
	}

	snake_body* glava = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t1 = (snake_body*) malloc(sizeof(snake_body));
	snake_body* t2 = (snake_body*) malloc(sizeof(snake_body));
	t2 -> x = WIDTH / 2  - 2; t2 -> y = HEIGHT / 2; t2 -> next = NULL;
	t1 -> x = WIDTH / 2 - 1; t1 -> y = HEIGHT / 2; t1 -> next = t2;
	glava -> x = WIDTH / 2; glava -> y = HEIGHT /2; glava -> next = t1;


	terminal_init();
	clear();
	setbuf(stdout, NULL);

	int* seed = (int*) malloc(sizeof(int));
	srand((int) seed);
	free (seed);

	draw_box(WIDTH * 2, HEIGHT, OFFSETX + 1, OFFSETY);
	apple* jabolko = ustvari_jabolko(glava, WIDTH, HEIGHT, OFFSETX + 3, OFFSETY + 2);

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
						if (ch + direction != 'B' && ch + direction != 'F' && ch - 'A' != direction) { // Checks that the directions are not opposite 'B' = 'B' + 0 = 'A' + 1 and 'F' = 'D' + 2 = 'c' + 3
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
		if (elapsed_ms > TICK_SPEED || (changed == 1 && elapsed_ms > 100)) {
			changed = 0;
			snake_body* nova_glava = premik(glava, direction, WIDTH, HEIGHT, &running, OFFSETX + 3, OFFSETY + 2);
			move_cursor(OFFSETX + 3 + nova_glava -> x * 2, OFFSETY + 2 + nova_glava -> y);
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
				else jabolko = ustvari_jabolko(nova_glava, WIDTH, HEIGHT, OFFSETX + 3, OFFSETY + 2);
			} else odstrani_zadnjo(nova_glava, OFFSETX + 3, OFFSETY + 2);
			move_cursor(OFFSETX + 3 + glava -> x * 2, OFFSETY + 2 + glava -> y);
			print_green(oblika);
			if (running) {
				move_cursor(OFFSETX + 3 + nova_glava -> x * 2, OFFSETY + 2 + nova_glava -> y);
				print_dark_green("██");
			}
			gettimeofday(&tv_before, NULL);
			glava = nova_glava;
			prev_direction = direction;
		}
	}
	move_cursor(OFFSETX + 3 + WIDTH * 2, OFFSETY + 2 + HEIGHT);
	printf("\n");
	terminal_cleanup();

    return 0;
}
