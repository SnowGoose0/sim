/* SIM command line argument parsing */

#include <ncurses.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "frame.c"
#include "keyboard.c"

/* SIM main function: entry point) */
int main(int argc, char** argv) {
  char opt;

  bool opt_new;
  bool opt_edit;
  bool opt_help;

  /* 
  while ((opt = getopt(argc, argv, "n:e:h")) != -1) {
    switch(opt) {
    case 'n':
      opt_new = true;
      break;

    case 'e':
      opt_edit = true;
      break;

    case 'h':
      opt_help = true;
      break;

    default:
      fprintf(stderr, "SIM ERROR: no arguments provided\n");
      exit(1);
    } }*/

    
    /*
      Help options has highest precedence than all other options

    */

  /*
    if (opt_help) {
      printf("help\n");
      exit(0);
    } */

    int mode = 1;
    
    Frame* fr = init_frame();
    int* mode_ptr = &mode;

    listen_in(fr, mode_ptr);

  
  exit(0);
}
