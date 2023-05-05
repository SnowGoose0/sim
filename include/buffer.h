#ifndef GAP_BUFF_H
#define GAP_BUFF_H

#define BUFF_GAP_SIZE                  0x20

#define LINE_FEED_CHAR                 0x0A 

#define SEARCH_DIRECTION_FORWARD       0x10
#define SEARCH_DIRECTION_BACKWARD      0x15

#define EMPTY_FILE                     "\0"

typedef struct {
  char* buffer;
  char* gap_front;
  char* gap_end;
  int buffer_viewable_front; /* offset: of the char in the corner of the screen  */
  int length;
  int cursor_line;
  int cursor_offset;
  int terminal_x;
} text_buffer;

text_buffer* init_text_buffer(char* buffer, int terminal_x);

int resize_buffer(text_buffer* t_buffer);

char* buffer_to_dstring(text_buffer* t_buffer);
char* buffer_to_string(text_buffer* t_buffer);
void _display_text_buffer(text_buffer* t_buffer);

void insertion_buffer(text_buffer* t_buffer, char symbol);
void deletion_buffer(text_buffer* t_buffer);

void move_buffer_cursor(text_buffer* t_buffer, int position);
void cursor_left_buffer(text_buffer* t_buffer, int offset);
void cursor_right_buffer(text_buffer* t_buffer, int offset);

void update_relative_cursor(text_buffer* t_buffer, int direction);
void update_buffer_viewable_front(text_buffer* t_buffer, int direction);

int buffer_cursor_at_eof(text_buffer* t_buffer);

int next_break(text_buffer* text, int direction);

void free_buffer(text_buffer* t_buffer);

#endif
