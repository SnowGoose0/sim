#ifndef MAIN_H
#define MAIN_H

#include <ncurses.h>

#include "screen.h"
#include "gap_buffer.h"

#define EMPTY_FILE "\0"

screen* init_screen();
void handle_commands_inputs(screen* scr, text_buffer* text, int ch);
void handle_text_inputs(screen* scr, text_buffer* text, int ch);
void insert_character(int ch, text_buffer* text, WINDOW* win, screen* scr);
void delete_character(int ch, text_buffer* text, WINDOW* win, screen* scr);

#endif
