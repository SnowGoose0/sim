#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "tutils.h"

// init
// gap_front points to the first _
// gap_end points to the first char after the last _
text_buffer* init_text_buffer(char* buffer, int terminal_x) {
  int buffer_length = strlen(buffer);
  int text_buffer_length = buffer_length + BUFF_GAP_SIZE;

  text_buffer* new_buffer = (text_buffer*) calloc(1, sizeof(text_buffer));
  new_buffer->buffer = (char*) calloc(text_buffer_length, sizeof(char));
  new_buffer->gap_front = new_buffer->buffer;
  new_buffer->gap_end = new_buffer->buffer + BUFF_GAP_SIZE;
  new_buffer->buffer_viewable_front = 0;
  
  new_buffer->length = text_buffer_length;
  new_buffer->cursor_line = 0;
  new_buffer->cursor_offset = 0;
  new_buffer->terminal_x = terminal_x;

  if (strcmp(buffer, "\0"))
    strcpy(new_buffer->gap_end, buffer);
  
  return new_buffer;
}

// insert
void insertion_buffer(text_buffer* t_buffer, char symbol) {
  if (t_buffer->gap_front == t_buffer->gap_end) {
    resize_buffer(t_buffer);
  }

  char* dest = t_buffer->gap_front;
  *dest = symbol;
  t_buffer->gap_front++;

  update_relative_cursor(t_buffer, SEARCH_DIRECTION_FORWARD);
}

// delete
void deletion_buffer(text_buffer* t_buffer) {
  if (t_buffer->buffer != t_buffer->gap_front) {
    t_buffer->gap_front--;

    update_relative_cursor(t_buffer, SEARCH_DIRECTION_BACKWARD);
  }
}

// grow
// this function should only be called if gap size is 0
int resize_buffer(text_buffer* t_buffer) {
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
    cursor_left_buffer(t_buffer, position);
    return;
  }

  cursor_right_buffer(t_buffer, position);
}

// left
void cursor_left_buffer(text_buffer* t_buffer, int offset) {
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
void cursor_right_buffer(text_buffer* t_buffer, int offset) {
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

/* forward and backward once */
void update_buffer_viewable_front(text_buffer* t_buffer, int direction) {
  char* start = buffer_to_string(t_buffer);
  char* start_viewable = start + t_buffer->buffer_viewable_front;
  char* tmp = start_viewable;

  int terminal_x = t_buffer->terminal_x;

  if (direction == SEARCH_DIRECTION_FORWARD) {
    while(*tmp != LINE_FEED_CHAR && tmp - start_viewable < terminal_x) {
      ++tmp;

      if (!*tmp) return;
    }

    if (*tmp == LINE_FEED_CHAR) tmp++;

    t_buffer->buffer_viewable_front = tmp - start;

  } else if (direction == SEARCH_DIRECTION_BACKWARD) {
    if (!t_buffer->buffer_viewable_front) return;
    
    if (*(--tmp) == LINE_FEED_CHAR) tmp -= 2;

    while(*tmp != LINE_FEED_CHAR
    //&& ABS(tmp - start_viewable) < terminal_x
    && tmp != start) {
      --tmp;
    }

    if (*tmp == LINE_FEED_CHAR) tmp++;

    int x = (start_viewable - tmp) % t_buffer->terminal_x;

    if (x == 0) x = t_buffer->terminal_x;

    // t_buffer->buffer_viewable_front = tmp - start;
    t_buffer->buffer_viewable_front = t_buffer->buffer_viewable_front - x;
  }
}

int buffer_cursor_at_eof(text_buffer* t_buffer) {
  if ((t_buffer->gap_end - t_buffer->buffer) == t_buffer->length)
    return 1;

  return 0;
}

char* buffer_to_string(text_buffer* t_buffer) {
  int n = t_buffer->length;
  char* buffer_str;

  buffer_str = (char*) calloc(n + 1, sizeof(char));
  char* tmp = buffer_str;
  char* src = t_buffer->buffer;

  while (n--) {
    if (t_buffer->gap_front <= src && src < t_buffer->gap_end) {
      ++src;
      continue;
    }
    *tmp++ = *src++;
  }

  return buffer_str;
}

char* buffer_to_dstring(text_buffer* t_buffer) {
  char* d_str = buffer_to_string(t_buffer);
  char* tmp = d_str;

  while (*tmp) {
    if (*tmp == '\n')
      *tmp = '$';
    ++tmp;
  }

  return d_str;
}

/* this function is for debugging purposes only */
void _display_text_buffer(text_buffer* t_buffer) {
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

  while(*tmp != LINE_FEED_CHAR && *tmp) {
    if (direction == SEARCH_DIRECTION_FORWARD) ++tmp;
    else --tmp;
    ++count;
  }

  return count;
}

// free
void free_buffer(text_buffer* t_buffer) {
  free(t_buffer->buffer);
  free(t_buffer);
}
