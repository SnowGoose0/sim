#ifndef MAIN_H
#define MAIN_H

#include <ncurses.h>

#include "screen.h"
#include "gap_buffer.h"

#define EMPTY_FILE "\0"

screen* init_screen();
void handle_commands_inputs(screen* scr, text_buffer* text, int ch);
void handle_text_inputs(screen* scr, text_buffer* text, int ch);
void move_cursor_left(screen* scr, text_buffer* text);
void move_cursor_right(screen* scr, text_buffer* text);
void insert_character(int ch, text_buffer* text, WINDOW* win, screen* scr);
void insert_next_line(text_buffer* text, WINDOW* win, screen* scr);
void delete_character(text_buffer* text, WINDOW* win, screen* scr);

#endif
