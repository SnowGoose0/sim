#ifndef UTILS_H
#define UTILS_H

#include <ncurses.h>
#include "screen.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MOD(a, b) ((a % b) < 0 ? ((a % b) + b) : (a % b))

void kill();
void render_window(screen* scr);
void handle_cursor_movement(screen* scr, int movement);
void handle_line_wrap(screen* scr, int operation);

#endif