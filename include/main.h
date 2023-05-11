#ifndef MAIN_H
#define MAIN_H

#include <ncurses.h>

#include "scr.h"
#include "buffer.h"

screen* init_screen();

void handle_commands_inputs(screen* scr, text_buffer* text, int ch);
void handle_text_inputs(screen* scr, text_buffer* text, int ch);
void handle_exit(screen* scr, text_buffer* text, char* buffer);

void scroll_screen(screen* scr, text_buffer* text, int direction);

void cursor_up(screen* scr, text_buffer* text);
void cursor_down(screen* scr, text_buffer* text);
void cursor_left(screen* scr, text_buffer* text);
void cursor_right(screen* scr, text_buffer* text);
void cursor_extreme(screen* scr, text_buffer* text, int direction);

void insert_character(int ch, text_buffer* text, WINDOW* win, screen* scr);
void insert_next_line(text_buffer* text, screen* scr);

void delete_character(text_buffer* text, WINDOW* win, screen* scr);

#endif
