/* SIM gap buffer: main data structure to contain all user inserts in text mode */


#include <ncurses.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>

/*

 */
typedef struct {
  short modified;
  char* buffer;
  char* gap_front;
  char* gap_end;
  int buffer_viewable_front; /* offset: of the char in the corner of the screen  */
  int length;
  int cursor_line;
  int cursor_offset;
  int terminal_x;
} Buffer;



/*
  IMPORTANT:

  All functions below prefixed with _ are "old" functions, some with
  undersireable design or features that may or may not be used in
  the future.

  It should be kept as a reference and should not be modified as the
  data structure of the text stored within the buffer is more or less
  independent of the logical Frame in SIM.

  Use all top level functions above as an interface.

 */


/*
  Initialize gap buffer:
  
  Gap_front points to the first _
  Gap_end points to the first char after the last _

  */
Buffer* _init_buffer(char* buffer, int terminal_x) {
  int buffer_length = strlen(buffer);
  int text_buffer_length = buffer_length + BUFF_GAP_SIZE;

  Buffer* new_buffer = (text_buffer*) calloc(1, sizeof(text_buffer));
  new_buffer->modified = 0;
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
void _ins_buffer(Buffer* t_buffer, char symbol) {
  if (t_buffer->gap_front == t_buffer->gap_end) {
    resize_buffer(t_buffer);
  }

  char* dest = t_buffer->gap_front;
  *dest = symbol;
  t_buffer->gap_front++;
  t_buffer->modified = 1;

  update_relative_cursor(t_buffer, SEARCH_DIRECTION_FORWARD);
}

// delete
void _del_buffer(Buffer* t_buffer) {
  if (t_buffer->buffer != t_buffer->gap_front) {
    t_buffer->gap_front--;
    t_buffer->modified = 1;

    update_relative_cursor(t_buffer, SEARCH_DIRECTION_BACKWARD);
  }
}

// grow
// this function should only be called if gap size is 0
int _resize_buffer(Buffer* t_buffer) {
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
void _move_cursor(Buffer* t_buffer, int position) {
  int current_position = (int)(t_buffer->buffer - t_buffer->gap_front);
  
  if (position < current_position) {
    cursor_left_buffer(t_buffer, position);
    return;
  }

  cursor_right_buffer(t_buffer, position);
}

// left
void _cursor_left_buffer(text_buffer* t_buffer, int offset) {
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
void _cursor_right_buffer(text_buffer* t_buffer, int offset) {
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

/*
  Relative cursor is the logical cursor used rendered in the window.

  It's position can be calculated by mimicking the frame's cursor
  traversal - allowing the cursor position to be saved if the user
  exits text mode.
  
 */
void _update_relative_cursor(Buffer* t_buffer, int direction) {
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
void _update_buffer_viewable_front(Buffer* t_buffer, int direction) {
  char* start = buffer_to_string(t_buffer);
  char* start_viewable = start + t_buffer->buffer_viewable_front;
  char* tmp = start_viewable;

  int terminal_x = t_buffer->terminal_x;
  

  /*
    Scroll down
  */
  if (direction == SEARCH_DIRECTION_FORWARD) {

    
    while(*tmp != LINE_FEED_CHAR && tmp - start_viewable < terminal_x) {
      ++tmp;

      /*
	If the current displayed line is the last line in the file
       */
      if (!*tmp) return;
    }

    /*
      If the new line ends with line_feed then increment once to the next character
    */
    if (*tmp == LINE_FEED_CHAR) tmp++;
    

    /*
      Update the new offset
     */
    t_buffer->buffer_viewable_front = tmp - start;


    
    /*
      Scroll up
    */  
  } else if (direction == SEARCH_DIRECTION_BACKWARD) {

    /* If the offset is 0 then it is the start of file => do nothing */
    if (!t_buffer->buffer_viewable_front) return;

    /* Viewable offsets stores the offset of a char that is either after a \n
       or after a viewable char

       Ex. hello\nworld => 'w' is at the offset location
       Ex. helloworld   => 'w' is at the offset location (assuming o is at the edge)
     */
    if (*(--tmp) == LINE_FEED_CHAR) --tmp;


    /*
      Get the entire paragraph
    */
    while(*tmp != LINE_FEED_CHAR && tmp != start) {
      --tmp;
    }

    /*
      Increment once so the current char isn't \n
    */
    if (*tmp == LINE_FEED_CHAR) tmp++;

    /* Get the remaining of the paragraph

       Assume screen width is 11

     \nhello_world
       hello_world
       hel

       This should return the length 3 of "hel"
     */
    int offset = (start_viewable - tmp) % t_buffer->terminal_x;

    
    /* If 0 then that means the entire line is viewable

       Ex.
       
     \nhello_world
       hello_world
       
     */
    if (offset == 0 && *tmp != LINE_FEED_CHAR) offset = t_buffer->terminal_x;

    /*
      Set new offset
    */
    t_buffer->buffer_viewable_front = t_buffer->buffer_viewable_front - offset;
  }

  free(start);
}

int _buffer_cursor_at_eof(Buffer* t_buffer) {
  if ((t_buffer->gap_end - t_buffer->buffer) == t_buffer->length)
    return 1;

  return 0;
}

char* _buffer_to_string(Buffer* t_buffer) {
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

char* _buffer_to_dstring(Buffer* t_buffer) {
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
void _display_text_buffer(Buffer* t_buffer) {
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

int _next_break(Buffer* t_buffer, int direction) {
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
void _free_buffer(Buffer* t_buffer) {
  free(t_buffer->buffer);
  free(t_buffer);
}

