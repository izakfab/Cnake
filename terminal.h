#ifndef TERMINAL_H

#define TERMINAL_H

#define ASCII_ESC 27

void move_cursor(int x, int y);

void clear();

void draw_box(int width, int height, int offx, int offy);

void terminal_init();

void terminal_cleanup();

void print_red(char text[]);

void print_green(char text[]);

void print_dark_green(char text[]);

int kbhit();

#endif
