#ifndef SCREEN_H
#define SCREEN_H

#define INSERT_MODE           0x00
#define COMMAND_MODE          0x01

#define OPERATION_INSERT      0x40
#define OPERATION_NEXT_LN     0x50
#define OPERATION_DELETE      0x90

#define MOVEMENT_FORWARD      0x45
#define MOVEMENT_BACKWARD     0x55
#define MOVEMENT_NEXT_LN      0x65
#define MOVEMENT_PREV_LN      0x75
#define MOVEMENT_UP           0x85
#define MOVEMENT_DOWN         0x95

#define KEY_ESC               0x1B
#define KEY_NEXT_LN           0x0A

#define TAB_LENGTH            0x04

#define SPACE_CHAR            0x20    
#define LEGAL_CHAR_LOW        0x20
#define LEGAL_CHAR_UP         0x7E

#include "buffer.h"

typedef struct {
  WINDOW* t_window;
  WINDOW* c_window;
  int terminal_y;
  int terminal_x;
  int mode;
  char* boundary;
} screen;

#endif
