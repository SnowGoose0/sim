#ifndef FILE_H
#define FILE_H

#include "buffer.h"

#define DEFAULT_BUFFER_SIZE  0x20
#define FILE_EXIST           0x01
#define FILE_NOT_EXIST       0x00

#define MAX_NAME_LIMIT       0xFF

typedef struct {
  char* file_name;
  char* file_path;
  char* file_content; /* initially read */
  int size;
} file;

file* new_file(char* indicator, int file_existence);
void destroy_file(file* f);

int write_file(file* file_data, char* file_string);
char* read_file(file* file_data);

#define FILE_SLASH_MSG         "File name cannot contain the blackslash character\n"
#define FILE_NAME_EXCEED_MSG   "File name is too long\n"
#define FILE_PATH_DNE          "File path %s does not exist\n"

#endif
