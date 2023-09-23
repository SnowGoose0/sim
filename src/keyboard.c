/* SIM user input event loop manager */

#include <ncurses.h>

#include <stdio.h>
#include <stdlib.h>

/*
  Parse and execute character entered in text mode

  WINDOW* win in this context represents the text window
 */
unsigned int exec_txt_in(WINDOW* win, char ch) {

  switch (ch) {

    
  default:
    winsch(win, ch);
  }
  return 0;
}

/*
  Parse and execute character entered in command  mode

  WINDOW* win in this context represents the command window
 */
unsigned int exec_cmd_in(WINDOW* win, char ch) {
  return 0;
}

/* Event loop entry point for SIM:

   Mode pointer created by main function and parsing depends
   on the the condition on mode (text or command mode)
 */
unsigned int listen_in(Frame* fr, int* mode) {
  for (;;) {
    char ch = getch();

    if (*mode) {
      if (exec_txt_in(fr->twin, ch)) {
	return 1;
      }
    }

    else {
      if (exec_cmd_in(fr->cwin, ch)) {
	return 1;
      }
    }
  }

  return 0;
}
