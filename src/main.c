#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ncurses.h>

#include "main.h"
#include "utils.h"
#include "gap_buffer.h"

int main(void) {
  int ch = 0;
  
  initscr();
  mousemask(0, NULL);
  noecho();
  
  screen* scr = init_screen();
  text_buffer* text = init_text_buffer(EMPTY_FILE, scr->terminal_x);

  // event loop
  while(1) {
    ch = getch();

    if (scr->mode == COMMAND_MODE)
      handle_commands_inputs(scr, text, ch);
    else
      handle_text_inputs(scr, text, ch);
  }
  
  kill();
}

screen* init_screen() {
  screen* new_screen = (screen*) malloc(sizeof(screen));
  new_screen->eof = 0;
  new_screen->mode = COMMAND_MODE;
  
  render_window(new_screen);
  return new_screen;

}

void handle_commands_inputs(screen* scr, text_buffer* text, int ch) {
  WINDOW* t_window = scr->t_window;
  // WINDOW* c_window = scr->c_window;
  
  switch(ch) {    
    case 'i':
      scr->mode = INSERT_MODE;
      wmove(t_window, 0, 0);
      move_cursor_sof(text);
      wrefresh(t_window);
      break;

    case 'q':
      kill();
      break;
      
    default:
      break;
  }
}

void handle_text_inputs(screen* scr, text_buffer* text, int ch) {
  WINDOW* c_window = scr->c_window;
  WINDOW* t_window = scr->t_window;
  char* display_buffer = NULL;
  
  int cursor_y, cursor_x;
  getyx(scr->t_window, cursor_y, cursor_x);

  int new_cursor_y = cursor_y;

  if (cursor_x == 0 && cursor_y != 0) {
    new_cursor_y--;
  } else if (cursor_x == scr->terminal_x - 1) {
    new_cursor_y++;
  }
  
  switch(ch) {
    case KEY_ESC:
      scr->mode = COMMAND_MODE;
      wmove(c_window, 0, 0);
      wclear(c_window);
      wprintw(c_window, "%s", get_debug_string(text));
      wrefresh(c_window);
      break;

    case '-':
      move_cursor_left(scr, text);
      wrefresh(t_window);
      break;

    case '=':
      move_cursor_right(scr, text);
      wrefresh(t_window);
      break;

    case KEY_BACKSPACE:
    case 127:
    case '\b':
      delete_character(text, t_window, scr);
      wrefresh(t_window);
      break;

    case KEY_ENTER:
    case KEY_NEXT_LN:
      insert_next_line(text, t_window, scr);
      wrefresh(t_window);
      break;

    case KEY_BTAB:
    case '\t':
      for (int i = 0; i < TAB_LENGTH; ++i)
	insert_character(SPACE_CHAR, text, t_window, scr);
      wrefresh(t_window);
      break;
   
    default:
      insert_character(ch, text, t_window, scr);
      wrefresh(t_window);
      break;
  }

  if (!display_buffer) free(display_buffer);
}

void move_cursor_left(screen* scr, text_buffer* text) {
  char ch = *(text->gap_front - 1);

  if (ch != '\n') {
    handle_terminal_cursor(scr, MOVEMENT_BACKWARD);
    cursor_left(text, 1);    
  }
}

void move_cursor_right(screen* scr, text_buffer* text) {
  char ch = *text->gap_end;

  if (!cursor_at_eof(text) && ch != '\n') {
    handle_terminal_cursor(scr, MOVEMENT_FORWARD);
    cursor_right(text, 1);    
  }
}

void insert_character(int ch, text_buffer* text, WINDOW* win, screen* scr) {
  if (LEGAL_CHAR_LOW <= ch && LEGAL_CHAR_UP >= ch) {
    int operation = OPERATION_INSERT;
    int movement = MOVEMENT_FORWARD;
    
    winsch(win, ch);
    handle_terminal_cursor(scr, movement);
    handle_line_wrap(scr, text, operation);
  }
  
  insert_text_buffer(text, (char) ch);
}

void insert_next_line(text_buffer* text, WINDOW* win, screen* scr) {
  handle_line_wrap(scr, text, OPERATION_NEXT_LN);
  handle_terminal_cursor(scr, MOVEMENT_NEXT_LN);
  insert_text_buffer(text, '\n');
}

void delete_character(text_buffer* text, WINDOW* win, screen* scr) {
  char ch = *(text->gap_front - 1);

  handle_terminal_cursor(scr, ch != '\n' ? MOVEMENT_BACKWARD : MOVEMENT_PREV_LN);
  wdelch(win);
  handle_line_wrap(scr, text, OPERATION_DELETE);

  delete_text_buffer(text);
}
