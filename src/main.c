#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

  // event loop
  while(1) {
    ch = getch();

    if (scr->mode == COMMAND_MODE)
      handle_commands_inputs(scr, ch);
    else
      handle_text_inputs(scr, ch);
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

void handle_commands_inputs(screen* scr, int ch) {
  WINDOW* t_window = scr->t_window;
  WINDOW* c_window = scr->c_window;
  
  switch(ch) {    
    case 'i':
      scr->mode = INSERT_MODE;
      wmove(t_window, 0, 0);
      wrefresh(t_window);
      break;

    case 'q':
      kill();
      break;
      
    default:
      break;
  }
}


void handle_text_inputs(screen* scr, int ch) {
  WINDOW* c_window = scr->c_window;
  WINDOW* t_window = scr->t_window;
  
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
      wrefresh(c_window);
      break;

    case '-':
      handle_cursor_movement(scr, MOVEMENT_BACKWARD);
      wrefresh(t_window);
      break;

    case '=':
      handle_cursor_movement(scr, MOVEMENT_FORWARD);
      wrefresh(t_window);
      break;

    case KEY_BACKSPACE:
    case 127:
    case '\b':
      handle_cursor_movement(scr, MOVEMENT_BACKWARD);
      wdelch(t_window);
      handle_line_wrap(scr, OPERATION_DELETE);
      wrefresh(t_window);
      break;

    case KEY_ENTER:
      scr->eof++;
      
      wrefresh(t_window);
      break;

    default:
      scr->eof++;
      handle_line_wrap(scr, OPERATION_INSERT);
      winsch(t_window, ch);
      handle_cursor_movement(scr, MOVEMENT_FORWARD);
      wrefresh(t_window);
      break;
  }
}
