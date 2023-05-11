#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "main.h"
#include "futils.h"
#include "tutils.h"
#include "desc.h"
#include "colors.h"
#include "buffer.h"

file* file_data;

screen* init_screen() {
  int terminal_y, terminal_x;
  getmaxyx(stdscr, terminal_y, terminal_x);
  
  screen* scr = (screen*) malloc(sizeof(screen));
  char* boundary = generate_boundary(terminal_y);

  WINDOW* text_window = newwin(terminal_y - 1, terminal_x, 0, 0);
  WINDOW* command_window = newwin(1, terminal_x, terminal_y - 1, 0);
  refresh();
  
  /* skip the first line feed */
  wprintw(text_window, "%s", file_data->file_content);
  print_attr(boundary, text_window, COLOR_PAIR(THEME_BOUNDARY));
  wrefresh(text_window);

  wmove(command_window, 0, 0);
  display_file_desc(file_data, command_window, 0);
  
  scr->mode = COMMAND_MODE;
  scr->t_window = text_window;
  scr->c_window = command_window;
  scr->terminal_y = terminal_y;
  scr->terminal_x = terminal_x;
  scr->s_cursor_y = 0;
  scr->s_cursor_x = 0;
  scr->boundary = boundary;
  
  return scr;
}

void handle_commands_inputs(screen* scr, text_buffer* text, int ch) {
  char* buffer = buffer = buffer_to_string(text);
  WINDOW* t_window = scr->t_window;
  WINDOW* c_window = scr->c_window;
  
  switch(ch) {
    case 'i':
      scr->mode = INSERT_MODE;
      cprint_command_attr(c_window, A_BOLD, "%s", INSERT_DESCR);
      wmove(t_window, scr->s_cursor_y, scr->s_cursor_x);
      
      wrefresh(c_window);
      wrefresh(t_window);
      break;
      
    case 'w':
      write_file(file_data, buffer);
      text->modified = 0;
      cprint_command_attr(c_window, COLOR_PAIR(THEME_OPERATION), "%dB %s", file_data->size, WRITTEN_DESCR);

      wrefresh(c_window);
      break;

    case CTRL('w'):
      write_file(file_data, buffer);
      kill(scr, text);
      break;

    case 'q':
      handle_exit(scr, text, buffer);
      break;
      
    default:
      break;
  }

  if (buffer != NULL) free(buffer);
}

void handle_exit(screen* scr, text_buffer* text, char* buffer) {
  WINDOW* c_window = scr->c_window;
  
  if (text->modified) {
    int c_ch;
    cprint_command_attr(c_window, A_REVERSE, "%s", MODIFIED_DESCR);

    while ((c_ch = wgetch(c_window)) != '!') {
      if ((c_ch == 'n')) {
	kill(scr, text);
	    
      } else if (c_ch == 'y') {
	write_file(file_data, buffer);
	free(buffer);
	kill(scr, text);	   
      }
    }

    display_file_desc(file_data, c_window, text->modified);
  }

  else kill(scr, text);
}
 
void handle_text_inputs(screen* scr, text_buffer* text, int ch) {
  WINDOW* c_window = scr->c_window;
  WINDOW* t_window = scr->t_window;
  char* display_buffer = NULL;
  
  int cursor_y, cursor_x;
  getyx(scr->t_window, cursor_y, cursor_x);
  
  switch(ch) {
    case KEY_ESC:
      scr->mode = COMMAND_MODE;
      scr->s_cursor_y = cursor_y;
      scr->s_cursor_x = cursor_x;
      
      wmove(c_window, 0, 0);
      display_file_desc(file_data, c_window, text->modified);
      break;

    case CTRL('k'):
      scroll_screen(scr, text, SEARCH_DIRECTION_BACKWARD);
      wrefresh(t_window);
      break;

    case CTRL('l'):
      scroll_screen(scr, text, SEARCH_DIRECTION_FORWARD);
      wrefresh(t_window);
      break;

    case CTRL('p'):
      if (cursor_y != 0) cursor_up(scr, text);
      else scroll_screen(scr, text, SEARCH_DIRECTION_BACKWARD);
      
      wrefresh(t_window);
      break;

    case CTRL('n'):
      if (cursor_y < scr->terminal_y - 2) cursor_down(scr, text);
      else scroll_screen(scr, text, SEARCH_DIRECTION_FORWARD);
      
      wrefresh(t_window);
      break;

    case CTRL('b'):
      cursor_left(scr, text);
      wrefresh(t_window);
      break;

    case CTRL('f'):
      cursor_right(scr, text);
      wrefresh(t_window);
      break;

    case CTRL('e'):
      cursor_extreme(scr, text, SEARCH_DIRECTION_FORWARD);
      wrefresh(t_window);
      break;

    case CTRL('a'):
      cursor_extreme(scr, text, SEARCH_DIRECTION_BACKWARD);
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
      insert_next_line(text, scr);
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

  display_cursor_location(scr, text);

  if (!display_buffer) free(display_buffer);
}

void scroll_screen(screen* scr, text_buffer* text, int direction) {
  int cursor_y = getcury(scr->t_window);
  
  char* focus_string;
  int viewable_prev = text->buffer_viewable_front;
  
  update_buffer_viewable_front(text, direction);

  if (direction == SEARCH_DIRECTION_FORWARD) {
    int text_cursor_y = text->cursor_line;
    cursor_down(scr, text);

    if (text->cursor_line == text_cursor_y) {
      handle_terminal_cursor(scr, text, MOVEMENT_UP);
      cursor_y = getcury(scr->t_window);
    }
      
  } else {
    
    if (text->buffer_viewable_front == viewable_prev) return;
    cursor_up(scr, text);
  }

  char* buffer = buffer_to_string(text);
  focus_string = buffer + text->buffer_viewable_front;

  wclear(scr->t_window);
  mvwprintw(scr->t_window, 0, 0, "%s", focus_string);
  print_attr(scr->boundary, scr->t_window, COLOR_PAIR(THEME_BOUNDARY));

  wmove(scr->t_window, cursor_y, text->cursor_offset);
  free(buffer);
}

void cursor_up(screen* scr, text_buffer* text) {
  int prev_line_offset = next_break(text, SEARCH_DIRECTION_BACKWARD);
  int offset = MIN(prev_line_offset + 1, scr->terminal_x);

  if (text->gap_front - offset < text->buffer) return;

  for (int i = 0; i < offset; i++)
    cursor_left(scr, text);
}


void cursor_down(screen* scr, text_buffer* text) {
  int next_line_offset = next_break(text, SEARCH_DIRECTION_FORWARD);
  int offset = MIN(next_line_offset + 1, scr->terminal_x);

  if (text->gap_end + offset > text->buffer + text->length) return;

  for (int i = 0; i < offset; i++)
    cursor_right(scr, text);
}


void cursor_left(screen* scr, text_buffer* text) {
  char ch = *(text->gap_front - 1);
  int movement = MOVEMENT_BACKWARD;
  
  cursor_left_buffer(text, 1);

  if (ch == LINE_FEED_CHAR)
    movement = MOVEMENT_PREV_LN;

  handle_terminal_cursor(scr, text, movement);
}

void cursor_right(screen* scr, text_buffer* text) {
  char ch = *text->gap_end;
  int movement = MOVEMENT_FORWARD;
  
  if (buffer_cursor_at_eof(text))
    return;
  
  cursor_right_buffer(text, 1);

  if (ch == LINE_FEED_CHAR)
    movement = MOVEMENT_NEXT_LN;

  handle_terminal_cursor(scr, text, movement);
}

void cursor_extreme(screen* scr, text_buffer* text, int direction) {
  int offset = next_break(text, direction);
  int i = 0;

  for (; i < offset; ++i) {
    if (direction == SEARCH_DIRECTION_FORWARD)
      cursor_right(scr, text);

    else if (direction == SEARCH_DIRECTION_BACKWARD)
      cursor_left(scr, text);
  }
}

void insert_character(int ch, text_buffer* text, WINDOW* win, screen* scr) {
  if (LEGAL_CHAR_LOW <= ch && LEGAL_CHAR_UP >= ch) {
    int operation = OPERATION_INSERT;
    int movement = MOVEMENT_FORWARD;
    
    insertion_buffer(text, (char) ch);
    winsch(win, ch);
    handle_terminal_cursor(scr, text, movement);
    handle_terminal_ops(scr, text, operation);
  }
}

void insert_next_line(text_buffer* text, screen* scr) {
  insertion_buffer(text, LINE_FEED_CHAR);
  handle_terminal_ops(scr, text, OPERATION_NEXT_LN);
  handle_terminal_cursor(scr, text, MOVEMENT_NEXT_LN);
}

void delete_character(text_buffer* text, WINDOW* win, screen* scr) {
  char ch = *(text->gap_front - 1);
  int movement = ch != LINE_FEED_CHAR ? MOVEMENT_BACKWARD : MOVEMENT_PREV_LN;
  
  int cursor_y, cursor_x;
  getyx(win, cursor_y, cursor_x);

  if ((!cursor_y && !cursor_x) && text->gap_front != text->buffer) {
    int c_offset = text->cursor_offset;
    int c_line = text->cursor_line;
    
    scroll_screen(scr, text, SEARCH_DIRECTION_BACKWARD);

    /* unscroll the terminal and text cursor */
    //wmove(scr->t_window, cursor_y + 1, cursor_x);
    while (text->cursor_offset != c_offset && text->cursor_line != c_line)
      cursor_right(scr, text);
  }

  deletion_buffer(text);
  handle_terminal_cursor(scr, text, movement);
  wdelch(win); 
  handle_terminal_ops(scr, text, OPERATION_DELETE);

}

int main(int argc, char** argv) {

  /*  Handle program options  */

  int opt = 0;
  int n_flag = 0, e_flag = 0, h_flag = 0, ur_flag = 0;

  while ((opt = getopt(argc, argv, ARGS)) != -1) {
    switch(opt) {
      case 'n':
	n_flag = 1;
	file_data = new_file(optarg, FILE_NOT_EXIST);
	break;
	
      case 'e':
	e_flag = 1;
	file_data = new_file(optarg, FILE_EXIST);
	break;

      case 'h':
	h_flag = 1;
	break;

      default:
	ur_flag = 1;
    }
  }

  if (h_flag) {
    time_t c_time = time(NULL);
    char* c_time_string = ctime(&c_time);
    *(c_time_string + strcspn(c_time_string, "\n")) = 0;
    
    printf(HELP_DESCR, c_time_string);
    printf(SPLASH);    
    exit(0);
  }

  if (ABS((n_flag ^ e_flag) - 1) | ur_flag) {
    fprintf(stderr, INVALID_FLAG_DESCR);
    printf(SPLASH);
    exit(1);
  }

  /*  Handle screen initialization  */
  
  initscr();
  start_color();
  mousemask(0, NULL);
  noecho();
  raw();

  init_pair(THEME_BOUNDARY, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(THEME_WINDOW, COLOR_WHITE, COLOR_BLACK);
  init_pair(THEME_OPERATION, COLOR_BLUE, COLOR_BLACK);

  screen* scr = init_screen();
  text_buffer* text = init_text_buffer(file_data->file_content, scr->terminal_x);
  int ch = 0;

  wbkgd(scr->t_window, COLOR_PAIR(THEME_WINDOW));
  wbkgd(scr->c_window, COLOR_PAIR(THEME_WINDOW));

  /* Event Loop */
  for(;;) {
    ch = getch();

    if (scr->mode == COMMAND_MODE)
      handle_commands_inputs(scr, text, ch);
    else
      handle_text_inputs(scr, text, ch);
  }
  
  kill(scr, text);
}
