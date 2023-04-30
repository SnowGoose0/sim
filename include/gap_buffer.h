#ifndef GAP_BUFF_H
#define GAP_BUFF_H

#define BUFF_GAP_SIZE 30

typedef struct {
  char* buffer;
  char* gap_front;
  char* gap_end;
  int length;
} text_buffer;

text_buffer* init_text_buffer(char* buffer);
char* get_debug_string(text_buffer* t_buffer);
char* get_focused_string(text_buffer* t_buffer);
void insert_text_buffer(text_buffer* t_buffer, char symbol);
void delete_text_buffer(text_buffer* t_buffer);
void move_cursor(text_buffer* t_buffer, int position);
void move_cursor_sof(text_buffer* t_buffer);
void cursor_left(text_buffer* t_buffer, int offset);
void cursor_right(text_buffer* t_buffer, int offset);
int resize_gap(text_buffer* t_buffer);
void display_text_buffer(text_buffer* t_buffer);
void free_text_buffer(text_buffer* t_buffer);

void hello(text_buffer* t_buffer);

#endif
