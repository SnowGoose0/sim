#include <stdlib.h>

#include "utils.h"
#include "gap_buffer.h"

void init_window(screen* scr) {
  int terminal_y, terminal_x;
  getmaxyx(stdscr, terminal_y, terminal_x);

  WINDOW* text_window = newwin(terminal_y - 1, terminal_x, 0, 0);
  WINDOW* command_window = newwin(1, terminal_x, terminal_y - 1, 0);

  keypad(text_window, TRUE);
  keypad(command_window, TRUE);

  refresh();

  // wborder(text_window, '~', ' ', ' ', ' ', '~', ' ', '~', ' ');
  wrefresh(text_window);

  wmove(command_window, 0, 0);
  box(command_window, 0, 0);
  wrefresh(command_window);

  scr->t_window = text_window;
  scr->c_window = command_window;
  scr->terminal_y = terminal_y;
  scr->terminal_x = terminal_x;
}

void handle_terminal_cursor(screen* scr, int movement) {
  WINDOW* t_window = scr->t_window;
  
  int cursor_y, cursor_x, new_cursor_y, new_cursor_x;
  getyx(scr->t_window, cursor_y, cursor_x);
  
  new_cursor_y = cursor_y;

  switch(movement) {
    case MOVEMENT_FORWARD:
      new_cursor_x = (cursor_x + 1) % scr->terminal_x;
    
      if (cursor_x == scr->terminal_x - 1)
	      new_cursor_y = MIN(scr->terminal_y, cursor_y + 1);
      
      break;

    case MOVEMENT_BACKWARD:
      new_cursor_x = cursor_x + cursor_y > 0
	      ? MOD(cursor_x -1, scr->terminal_x)
	      : cursor_x;
      
      if (cursor_x == 0)
	      new_cursor_y = MAX(0, cursor_y - 1);
      
      break;

    case MOVEMENT_NEXT_LN:
      new_cursor_y = cursor_y + 1;
      new_cursor_x = 0;
      break;

    case MOVEMENT_PREV_LN:
      new_cursor_y = 0;
      new_cursor_x = 0;

    default:
      new_cursor_y = cursor_y;
      new_cursor_x = cursor_x;
      break;
  }

  wmove(t_window, new_cursor_y, new_cursor_x);
}

void handle_terminal_format(screen* scr, text_buffer* text, int operation) {
  WINDOW* t_window = scr->t_window;

  int cursor_y, cursor_x;
  getyx(t_window, cursor_y, cursor_x);

  char* focused_string = get_focused_string(text);
  char* bottom_string = focused_string + (text->gap_front - text->buffer);
  char* format_string;
  
  wclrtobot(t_window);
  switch(operation) {
    case OPERATION_INSERT:
    case OPERATION_DELETE:
      format_string = "%s";
      break;

    case OPERATION_NEXT_LN:
      format_string = "\n%s";
      break;

    default:
      format_string = NULL;
  }

  wprintw(t_window, format_string, bottom_string); 
  wmove(t_window, cursor_y, cursor_x);
  free(focused_string);
}

void kill() {
  system("clear");
  endwin();
  exit(0);
}
