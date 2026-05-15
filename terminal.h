#include "Cnake.h"

#ifndef TERMINAL_H

#define TERMINAL_H

#define ASCII_ESC 27

void move_cursor(int x, int y);

void clear();

void draw_box(int width, int height, int offx, int offy);

void draw_snake(snake_body* body, int offx, int offy, int prva);

void draw_button(char* text, int offx, int offy);

void select_button(char* text, int off, int offy);

void terminal_init();

void terminal_cleanup();

void print_red(char text[]);

void print_green(char text[]);

void print_dark_green(char text[]);

int kbhit();

#endif
