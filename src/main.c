#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "main.h"
#include "messages.h"
#include "utils.h"
#include "colors.h"
#include "gap_buffer.h"

int main(int argc, char** argv) {

  int opt = 0;
  int n_flag = 0, e_flag = 0, h_flag = 0, ur_flag = 0;

  while ((opt = getopt(argc, argv, ARGS)) != -1) {
    switch(opt) {
      case 'n':
	n_flag = 1;
	break;
	
      case 'e':
	e_flag = 1;
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
    
    exit(0);
  }

  if (~(n_flag ^ e_flag) | ur_flag) {
    printf(INVALID_FLAG_DESCR);
    
    exit(1);
  }
  
  initscr();
  start_color();
  mousemask(0, NULL);
  noecho();
  raw();

  init_pair(THEME_BOUNDARY, COLOR_GREEN, COLOR_BLACK);
  init_pair(THEME_WINDOW, COLOR_WHITE, COLOR_BLACK);

  int ch = 0;
  screen* scr = init_screen();
  text_buffer* text = init_text_buffer(EMPTY_FILE, scr->terminal_x);

  wbkgd(scr->t_window, COLOR_PAIR(THEME_WINDOW));
  wbkgd(scr->c_window, COLOR_PAIR(THEME_WINDOW));

  // event loop
  while(1) {
    ch = getch();

    if (scr->mode == COMMAND_MODE)
      handle_commands_inputs(scr, text, ch);
    else
      handle_text_inputs(scr, text, ch);
  }
  
  kill(scr, text);
}

screen* init_screen() {
  screen* new_screen = (screen*) malloc(sizeof(screen));
  new_screen->eof = 0;
  new_screen->mode = COMMAND_MODE;
  
  init_render_window(new_screen);
  return new_screen;
}

void handle_commands_inputs(screen* scr, text_buffer* text, int ch) {
  WINDOW* t_window = scr->t_window;
  WINDOW* c_window = scr->c_window;
  
  switch(ch) {    
    case 'i':
      scr->mode = INSERT_MODE;
      werase(c_window);
      print_attr("[ INSERT ]", c_window, A_BOLD);
      wrefresh(c_window);

      wmove(t_window, 0, 0);
      move_cursor_sof(text);
      wrefresh(t_window);

      break;

    case 'q':
      kill(scr, text);
      break;
      
    default:
      break;
  }
}

void handle_text_inputs(screen* scr, text_buffer* text, int ch) {
  WINDOW* c_window = scr->c_window;
  WINDOW* t_window = scr->t_window;
  char* display_buffer = NULL;
  
  //  int cursor_y, cursor_x;
  //  getyx(scr->t_window, cursor_y, cursor_x);
  
  switch(ch) {
    case KEY_ESC:
      scr->mode = COMMAND_MODE;
      wmove(c_window, 0, 0);
      wclear(c_window);
      
      char* debug_string = get_debug_string(text);
      wprintw(c_window, "%s", debug_string);
      free(debug_string);
      
      wrefresh(c_window);
      break;

    case '-':
      cursor_left(scr, text);
      wrefresh(t_window);
      break;

    case '=':
      cursor_right(scr, text);
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

  if (!display_buffer) free(display_buffer);
}

void cursor_left(screen* scr, text_buffer* text) {
  char ch = *(text->gap_front - 1);

  if (ch != LINE_FEED_CHAR) {
    handle_terminal_cursor(scr, text, MOVEMENT_BACKWARD);
    text_cursor_left(text, 1);    
  }
}

void cursor_right(screen* scr, text_buffer* text) {
  char ch = *text->gap_end;

  if (!text_cursor_at_eof(text) && ch != LINE_FEED_CHAR) {
    handle_terminal_cursor(scr, text, MOVEMENT_FORWARD);
    text_cursor_right(text, 1);    
  }
}

void insert_character(int ch, text_buffer* text, WINDOW* win, screen* scr) {
  if (LEGAL_CHAR_LOW <= ch && LEGAL_CHAR_UP >= ch) {
    int operation = OPERATION_INSERT;
    int movement = MOVEMENT_FORWARD;
    
    insert_text_buffer(text, (char) ch);
    winsch(win, ch);
    handle_terminal_cursor(scr, text, movement);
    handle_terminal_format(scr, text, operation);
  }
}

void insert_next_line(text_buffer* text, screen* scr) {
  insert_text_buffer(text, LINE_FEED_CHAR);
  handle_terminal_format(scr, text, OPERATION_NEXT_LN);
  handle_terminal_cursor(scr, text, MOVEMENT_NEXT_LN);
}

void delete_character(text_buffer* text, WINDOW* win, screen* scr) {
  char ch = *(text->gap_front - 1);

  delete_text_buffer(text);
  handle_terminal_cursor(scr, text, ch != LINE_FEED_CHAR ? MOVEMENT_BACKWARD : MOVEMENT_PREV_LN);
  wdelch(win);
  handle_terminal_format(scr, text, OPERATION_DELETE);
}
