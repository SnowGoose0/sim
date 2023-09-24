/* SIM ncurses window (frame) manager */

#include <ncurses.h>

#include <stdio.h>
#include <stdlib.h>

/* Frame structure:

   A structure used to keep track of various frame and cursor
   metadata and also stores the window pointers to the text
   and command window.
   
 */
typedef struct {
  WINDOW* twin;
  WINDOW* cwin;
  int fr_y; /* terminal frame dimensions */
  int fr_x;
  int cur_y;
  int cur_x;
} Frame;

unsigned int render_frame(Frame* fr, int y, int x);

/* Frame initialization is detatched from frame rendering as
   there may be a need for frame resizing upon user events.   
 */
Frame* init_frame() {
  Frame* fr = (Frame*) malloc(sizeof(Frame));
  
  int f_x;
  int f_y;

  getmaxyx(stdscr, f_x, f_y);

  fr->fr_y = f_y;
  fr->fr_x = f_x;
  fr->cur_y = 0;
  fr->cur_x = 0;

  render_frame(fr, f_y, f_x);

  return fr;
}

unsigned int render_frame(Frame* fr, int y, int x) {

  /* Initialize input window:

     Text window height is exactly one line shorter than actual
     terminal window's height (y) - last line is reserved for
     the command window.

     newwin takes arguments [nline, ncols, begin_y, begin_x]
     
   */
  WINDOW* txt_win = newwin(y - 1, x, 0, 0);
  WINDOW* cmd_win = newwin(1 , x, y - 1, 0);


  if (!txt_win || !cmd_win) {
    printf("SIM ERROR: screen initialization failed");    
    return 1;
  }
  
  fr->twin = txt_win;
  fr->cwin = cmd_win;
  
  return 0;
}
