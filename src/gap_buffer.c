#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

// init
// gap_front points to the first _
// gap_end points to the first char after the last _
text_buffer* init_text_buffer(char* buffer, int terminal_x) {
  int buffer_length = strlen(buffer);
  int text_buffer_length = buffer_length + BUFF_GAP_SIZE;

  text_buffer* new_buffer = (text_buffer*) malloc(sizeof(text_buffer));
  new_buffer->buffer = (char*) calloc(text_buffer_length, sizeof(char));
  new_buffer->gap_front = new_buffer->buffer;
  new_buffer->gap_end = new_buffer->buffer + BUFF_GAP_SIZE;
  new_buffer->length = text_buffer_length;
  new_buffer->cursor_line = 0;
  new_buffer->cursor_offset = 0;
  new_buffer->terminal_x = terminal_x;

  if (strcmp(buffer, "\0"))
    strcpy(new_buffer->gap_end, buffer);
  
  return new_buffer;
}

// insert
void insert_text_buffer(text_buffer* t_buffer, char symbol) {
  if (t_buffer->gap_front == t_buffer->gap_end) {
    resize_gap(t_buffer);
  }

  char* dest = t_buffer->gap_front;
  *dest = symbol;
  t_buffer->gap_front++;

  update_relative_cursor(t_buffer, SEARCH_DIRECTION_FORWARD);
}

// delete
void delete_text_buffer(text_buffer* t_buffer) {
  if (t_buffer->buffer != t_buffer->gap_front) {
    t_buffer->gap_front--;

    update_relative_cursor(t_buffer, SEARCH_DIRECTION_BACKWARD);
  }
}

// grow
// this function should only be called if gap size is 0
int resize_gap(text_buffer* t_buffer) {
  char* tmp = t_buffer->buffer;
  
  int new_buffer_length = BUFF_GAP_SIZE + t_buffer->length;
  char* new_buffer = (char*) calloc(new_buffer_length, sizeof(char));
  
  char* source_buffer = tmp;
  
  int front_length = (int)(t_buffer->gap_front - source_buffer);
  int back_length = (int)(t_buffer->buffer + t_buffer->length - t_buffer->gap_end);
  strncpy(new_buffer, source_buffer, front_length);
  strncpy(new_buffer + front_length + BUFF_GAP_SIZE, t_buffer->gap_end, back_length);

  free(tmp);
  t_buffer->buffer = new_buffer;
  t_buffer->gap_front = new_buffer + front_length;
  t_buffer->gap_end = t_buffer->gap_front + BUFF_GAP_SIZE;
  t_buffer->length = new_buffer_length;

  return 0;
}

// move
void move_cursor(text_buffer* t_buffer, int position) {
  int current_position = (int)(t_buffer->buffer - t_buffer->gap_front);
  
  if (position < current_position) {
    text_cursor_left(t_buffer, position);
    return;
  }

  text_cursor_right(t_buffer, position);
}

// left
void text_cursor_left(text_buffer* t_buffer, int offset) {
  char* buffer_front = t_buffer->buffer;
  
  for (int cur = 0; cur < offset; cur++) {
    if (t_buffer->gap_front == buffer_front) return;
    
    char tmp = *(t_buffer->gap_front - 1);
    t_buffer->gap_front--;
    t_buffer->gap_end--;
    *(t_buffer->gap_end) = tmp;

    update_relative_cursor(t_buffer, SEARCH_DIRECTION_BACKWARD);
  }
}

// right
void text_cursor_right(text_buffer* t_buffer, int offset) {
  char* buffer_back = t_buffer->buffer + t_buffer-> length;

  for (int cur = 0; cur < offset; cur++) {
    if (t_buffer->gap_end == buffer_back) return;
    
    char tmp = *(t_buffer->gap_end);
    t_buffer->gap_front++;
    t_buffer->gap_end++;
    *(t_buffer->gap_front - 1) = tmp;

    update_relative_cursor(t_buffer, SEARCH_DIRECTION_FORWARD);
  }
}

void update_relative_cursor(text_buffer* t_buffer, int direction) {
  char symbol = *(t_buffer->gap_front - 1);

  if (direction == SEARCH_DIRECTION_FORWARD) {
    if (symbol == LINE_FEED_CHAR || t_buffer->terminal_x - 1 == t_buffer->cursor_offset) {
      t_buffer->cursor_offset = 0;
      t_buffer->cursor_line++;
      
    } else t_buffer->cursor_offset++;
    
    return;
  }

  if (direction == SEARCH_DIRECTION_BACKWARD) {
    if (t_buffer->cursor_offset == 0 && t_buffer->cursor_line != 0) {
      int nearest_break = next_break(t_buffer, SEARCH_DIRECTION_BACKWARD);
      
      t_buffer->cursor_offset = nearest_break % t_buffer->terminal_x;
      t_buffer->cursor_line--;
      
    } else t_buffer->cursor_offset--;
  }
}

void move_cursor_sof(text_buffer* t_buffer) {
  char* front = t_buffer->buffer;
  
  while(t_buffer->gap_front != front)
    text_cursor_left(t_buffer, 1);
}

int text_cursor_at_eof(text_buffer* t_buffer) {
  if ((t_buffer->gap_end - t_buffer->buffer) == t_buffer->length)
    return 1;

  return 0;
}

// display
// this function is only for debugging purposes
void display_text_buffer(text_buffer* t_buffer) {
  char* buffer = t_buffer->buffer;
  int is_gap = 0;

  printf("Buffer Length: %d\n", t_buffer->length);
  printf("Cursor: Line %d, Offset, %d\n", t_buffer->cursor_line, t_buffer->cursor_offset);

  for (int i = 0; i < t_buffer->length; i++) {
    if (buffer + i == t_buffer->gap_front) is_gap = 1;
    if (buffer + i == t_buffer->gap_end) is_gap = 0;
    
    if (!is_gap) {
      char ch = *(buffer + i);
      if (ch == '\n') ch = '&';
      printf("%c ", ch);
      continue;
    }

    printf("_ ");
  }

  printf("\n\n");
}

int next_break(text_buffer* t_buffer, int direction) {
  char* tmp;
  int count = 0;
  
  if (direction == SEARCH_DIRECTION_FORWARD) tmp = t_buffer->gap_end;
  else tmp = t_buffer->gap_front - 1;

  while(*tmp != LINE_FEED_CHAR && *tmp != 0) {
    if (direction == SEARCH_DIRECTION_FORWARD) ++tmp;
    else --tmp;
    ++count;
  }

  return count;
}

// free
void free_text_buffer(text_buffer* t_buffer) {
  free(t_buffer->buffer);
  free(t_buffer);
}

char* get_debug_string(text_buffer* t_buffer) {
  int len = t_buffer->length + 1;
  char* buffer = t_buffer->buffer;
  char* str = (char*) calloc(len, sizeof(char));

  char* ignore_bound_low = t_buffer->gap_front;
  char* ignore_bound_up = t_buffer->gap_end;

  int str_index = 0;
  for(int i = 0; i < len; i++) {
    char* cur = buffer + i;
    char ch = *cur;

    if (ignore_bound_low <= cur && cur < ignore_bound_up) continue;
    if (ch == LINE_FEED_CHAR) ch = '&';
 
    str[str_index] = ch;
    str_index++;
  }

  *(str + str_index) = 0;
  return str;
}

char* get_focused_string(text_buffer* t_buffer) {
  char* f_string = get_debug_string(t_buffer);
  char* tmp = f_string;

  while(*tmp != 0) {
    if (*tmp == '&') *tmp = LINE_FEED_CHAR;
    tmp++;
  }

  return f_string;
}
