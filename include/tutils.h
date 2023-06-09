#ifndef tutils_H
#define tutils_H

#include <stdarg.h>
#include <ncurses.h>

#include "futils.h"
#include "scr.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MOD(a, b) ((a % b) < 0 ? ((a % b) + b) : (a % b))

void kill(screen* scr, text_buffer* text);

char* generate_boundary(int terminal_y);
char* display_file_desc(file* file_data, WINDOW* c_window, short modified);

void handle_terminal_cursor(screen* scr, text_buffer* text, int movement);
void handle_terminal_ops(screen* scr, text_buffer* text, int operation);

void display_cursor_location(screen* scr, text_buffer* text);
void display_boundaries(int cur_line, int terminal_y, WINDOW* t_window);
void print_attr(char* content, WINDOW* win, chtype attr);
void vprint_attr(WINDOW* win, chtype attr, char* content, ...);
void cprint_command_attr(WINDOW* c_window, chtype attr, const char* content, ...);
void print_command_attr(WINDOW* c_window, chtype attr, const char* content);

#endif
