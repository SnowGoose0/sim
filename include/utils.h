#ifndef UTILS_H
#define UTILS_H

#include <ncurses.h>
#include "screen.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MOD(a, b) ((a % b) < 0 ? ((a % b) + b) : (a % b))

void kill();
void init_window(screen* scr);

void handle_terminal_cursor(screen* scr, int movement);
void handle_terminal_format(screen* scr, text_buffer* text, int operation);

#endif
