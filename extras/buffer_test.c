#include <stdio.h>
#include "../src/gap_buffer.c"

void init_test();
void insert_test();
void left_right_test();
void empty_insert();

int main () {
  empty_insert();
  return 0;
}

void empty_insert() {
  char* empty = "\0";

  text_buffer* tb_1 = init_text_buffer(empty);
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'c');
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'c');
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'c');
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'c');
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'c');
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'i');
  display_text_buffer(tb_1);  
}

void init_test() {
  char* sample_1 = "Hello World!";
  char* sample_2 = "C";
  char* sample_3 = "\0";

  text_buffer* tb_1 = init_text_buffer(sample_1);
  display_text_buffer(tb_1);

  text_buffer* tb_2 = init_text_buffer(sample_2);
  display_text_buffer(tb_2);

  text_buffer* tb_3 = init_text_buffer(sample_3);
  display_text_buffer(tb_3);

  free_text_buffer(tb_1);
  free_text_buffer(tb_2);
  free_text_buffer(tb_3);
}

void insert_test() {
  char* sample_1 = "Hello World!";

  text_buffer* tb_1 = init_text_buffer(sample_1);
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'B');
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'A');
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'D');
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'C');
  display_text_buffer(tb_1);

  free_text_buffer(tb_1);
}

void left_right_test() {
  char* sample = "Hello World!";

  text_buffer* tb_1 = init_text_buffer(sample);

  cursor_right(tb_1, 1);
  display_text_buffer(tb_1);

  cursor_right(tb_1, 3);
  display_text_buffer(tb_1);

  cursor_left(tb_1, 1);
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'A');
  insert_text_buffer(tb_1, 'B');
  insert_text_buffer(tb_1, 'C');
  insert_text_buffer(tb_1, 'D');
  insert_text_buffer(tb_1, 'E');
  insert_text_buffer(tb_1, 'F');
  
  display_text_buffer(tb_1);

  cursor_right(tb_1, 9);
  display_text_buffer(tb_1);
  cursor_right(tb_1, 9);
  display_text_buffer(tb_1);

  insert_text_buffer(tb_1, 'E');
  insert_text_buffer(tb_1, 'N');
  insert_text_buffer(tb_1, 'D');
  insert_text_buffer(tb_1, 'D');
  insert_text_buffer(tb_1, 'X');

  display_text_buffer(tb_1);

  cursor_left(tb_1, 40);
  insert_text_buffer(tb_1, 'F');
  display_text_buffer(tb_1);


  insert_text_buffer(tb_1, 'E');
  insert_text_buffer(tb_1, 'N');
  insert_text_buffer(tb_1, 'D');
  insert_text_buffer(tb_1, 'D');
  insert_text_buffer(tb_1, 'X');

  display_text_buffer(tb_1);

  delete_text_buffer(tb_1);
  display_text_buffer(tb_1);

  free_text_buffer(tb_1);
}

