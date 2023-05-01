#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#include <ncurses.h>
#include "screen.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MOD(a, b) ((a % b) < 0 ? ((a % b) + b) : (a % b))

void kill(screen* scr, text_buffer* text);
void init_render_window(screen* scr);

void handle_terminal_cursor(screen* scr, text_buffer* text, int movement);
void handle_terminal_format(screen* scr, text_buffer* text, int operation);

char* generate_boundary(int terminal_y);
void print_boundaries(int cur_line, int terminal_y, WINDOW* t_window);
void print_attr(char* content, WINDOW* win, chtype attr);
void vprint_attr(WINDOW* win, chtype attr, char* content, ...);

#endif
