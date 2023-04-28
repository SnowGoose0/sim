#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MOD(a, b) ((a % b) < 0 ? ((a % b) + b) : (a % b))

#define INSERT_MODE 0x00
#define COMMAND_MODE 0x01
#define OPERATION_INSERT 0x45
#define OPERATION_DELETE 0x90
#define MOVEMENT_FORWARD 0x40
#define MOVEMENT_BACKWARD 0x85
#define KEY_ESC 27

typedef struct {
  WINDOW* t_window;
  WINDOW* c_window;

  int mode;
  int eof;
  int ch;

  int terminal_y;
  int terminal_x;
} screen;

screen* init_screen();
void kill();
void render_window(screen* scr);
void handle_commands_inputs(screen* scr, int ch);
void handle_text_inputs(screen* scr, int ch);
void handle_cursor_movement(screen* scr, int movement);
void handle_line_wrap(screen* scr, int operation);
void insert_ch(int ch);

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
  new_screen->ch = 0;
  new_screen->eof = 0;
  new_screen->mode = COMMAND_MODE;

  render_window(new_screen);
  return new_screen;
}

void render_window(screen* scr) {
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

    case KEY_RIGHT:
      handle_cursor_movement(scr, MOVEMENT_FORWARD);
      wrefresh(t_window);
      break;

    case KEY_BACKSPACE:
    case 127:
    case '\b':
      handle_cursor_movement(scr, MOVEMENT_BACKWARD);
      wdelch(t_window);
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

void handle_cursor_movement(screen* scr, int movement) {
  WINDOW* t_window = scr->t_window;
  
  int cursor_y, cursor_x, new_cursor_y, new_cursor_x;
  getyx(scr->t_window, cursor_y, cursor_x);
  new_cursor_y = cursor_y;
  
  if (movement == MOVEMENT_FORWARD) {
    new_cursor_x = (cursor_x + 1) % scr->terminal_x;
    
    if (cursor_x == scr->terminal_x - 1)
      new_cursor_y = MIN(scr->terminal_y, cursor_y + 1);

  } else if (movement == MOVEMENT_BACKWARD) {
      new_cursor_x = MOD(cursor_x -1, scr->terminal_x);
    
    if (cursor_x == 0)
      new_cursor_y = MAX(0, cursor_y - 1);
  }

  wmove(t_window, new_cursor_y, new_cursor_x);
}

void handle_line_wrap(screen* scr, int operation) {
  WINDOW* t_window = scr->t_window;

  int max_rows = scr->terminal_y;
  int cursor_y, cursor_x;
  getyx(scr->t_window, cursor_y, cursor_x);
    
  for (int row = max_rows - 1; row >= cursor_y; row--) {
    wmove(t_window, row, scr->terminal_x - 1);
    char ch = winch(t_window);
    wdelch(t_window);

    wmove(t_window, row + 1, 0);
    winsch(t_window, ch);
  }

  wmove(t_window, cursor_y, cursor_x);
}

void kill() {
  system("clear");
  endwin();
  exit(0);
}

