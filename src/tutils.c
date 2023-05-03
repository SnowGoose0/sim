#include <stdlib.h>
#include <ncurses.h>

#include "tutils.h"
#include "futils.h"
#include "colors.h"
#include "buffer.h"

void init_render_window(screen* scr, file* file_data) {
  int terminal_y, terminal_x;
  getmaxyx(stdscr, terminal_y, terminal_x);

  WINDOW* text_window = newwin(terminal_y - 1, terminal_x, 0, 0);
  WINDOW* command_window = newwin(1, terminal_x, terminal_y - 1, 0);
  refresh();

  keypad(text_window, TRUE);
  keypad(command_window, TRUE);

  char* boundary = generate_boundary(terminal_y);
  
  /* skip the first line feed */
  wprintw(text_window, "%s", file_data->file_content);
  print_attr(boundary, text_window, COLOR_PAIR(THEME_BOUNDARY));
  wrefresh(text_window);

  wmove(command_window, 0, 0);
  wrefresh(command_window);

  scr->t_window = text_window;
  scr->c_window = command_window;
  scr->terminal_y = terminal_y;
  scr->terminal_x = terminal_x;
  scr->s_cursor_y = 0;
  scr->s_cursor_x = 0;
  scr->boundary = boundary;
}

void handle_terminal_cursor(screen* scr, text_buffer* text, int movement) {
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
      new_cursor_y = MAX(0, cursor_y - 1);
      new_cursor_x = text->cursor_offset;
      break;

    default:
      new_cursor_y = cursor_y;
      new_cursor_x = cursor_x;
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
  print_attr(scr->boundary, t_window, COLOR_PAIR(THEME_BOUNDARY));
  print_line_count(scr, text);
  wmove(t_window, cursor_y, cursor_x);
  wrefresh(t_window);
  free(focused_string);
}

void print_line_count(screen* scr, text_buffer* text) {
  WINDOW* c_window = scr->c_window;
  char* coordinate_format = "L:%d C:%d";

  int loc_y = text->cursor_line + 1;
  int loc_x = text->cursor_offset + 1;

  wmove(c_window, 0, scr->terminal_x - 24);
  wclrtobot(c_window);
  wprintw(c_window, coordinate_format, loc_y, loc_x);
  wrefresh(c_window);
}

char* generate_boundary(int terminal_y) {
  char* boundary = (char*) calloc((2 * terminal_y) + 1, sizeof(char));
  
  for (int i = 0; i < 2 * terminal_y; i += 2) {
    *(boundary + i) = '\n';
    *(boundary + i + 1) = '~';
  }

  return boundary;
}

void print_attr(char* content, WINDOW* win, chtype attr) {
  wattron(win, attr);
  wprintw(win, "%s", content);
  wattroff(win, attr);
}

void kill(screen* scr, text_buffer* text) {
  endwin();
  delwin(scr->t_window);
  delwin(scr->c_window);
  
  free(scr->boundary);
  free(scr);
  free_text_buffer(text);
  
  system("clear");
  exit(0);
}
