/* SIM user input event loop manager */

#include <ncurses.h>

#include <stdio.h>
#include <stdlib.h>

unsigned int exec_txt_in(char ch) {
  
  
  return 0;
}

unsigned int exec_cmd_in(char ch) {
  return 0;
}

/* main event loop to listen to keyboard events */
unsigned int listen_in(int* mode) {
  for (;;) {
    ch = getch();

    if (*mode) {
      if (exec_txt_in(ch)) {
	return 1;
      }
    }

    else {
      if (exec_cmd_in(ch)) {
	return 1;
      }
    }
  }

  return 0;
}
