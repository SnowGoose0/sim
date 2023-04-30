#ifndef GAP_BUFF_H
#define GAP_BUFF_H

#define BUFF_GAP_SIZE 30

#define LINE_FEED_CHAR 0x0A

#define SEARCH_DIRECTION_FORWARD 0x10
#define SEARCH_DIRECTION_BACKWARD 0x15

typedef struct {
  char* buffer;
  char* gap_front;
  char* gap_end;
  int length;
  int cursor_line;
  int cursor_offset;
  int terminal_x;
} text_buffer;

text_buffer* init_text_buffer(char* buffer, int terminal_x);

char* get_debug_string(text_buffer* t_buffer);
char* get_focused_string(text_buffer* t_buffer);
void display_text_buffer(text_buffer* t_buffer);

void insert_text_buffer(text_buffer* t_buffer, char symbol);
void delete_text_buffer(text_buffer* t_buffer);

void move_text_cursor(text_buffer* t_buffer, int position);
void move_cursor_sof(text_buffer* t_buffer);
void text_cursor_left(text_buffer* t_buffer, int offset);
void text_cursor_right(text_buffer* t_buffer, int offset);
int text_cursor_at_eof(text_buffer* t_buffer);

int resize_gap(text_buffer* t_buffer);

int next_break(text_buffer* text, int direction);

void free_text_buffer(text_buffer* t_buffer);
void hello(text_buffer* t_buffer);

#endif
