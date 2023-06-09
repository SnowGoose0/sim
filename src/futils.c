#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "futils.h"

file* new_file(char* file_indicator, int file_existence) {
  file* f = (file*) malloc(sizeof(file));
  f->file_name = NULL;
  f->file_path = NULL;

  if (file_existence) {
    if (access(file_indicator, F_OK) != 0) {
      fprintf(stderr, FILE_PATH_DNE, file_indicator);
      exit(1);
    }
      
    f->file_path = file_indicator;
    f->file_content = read_file(f);
    f->size = strlen(f->file_content);
    
  } else {
    char* tmp = file_indicator;
    for (; *tmp; ++tmp) {
      if (*tmp == '/') {
	fprintf(stderr, FILE_SLASH_MSG);
	exit(1);
      }
    }

    if (tmp - file_indicator > MAX_NAME_LIMIT) {
      fprintf(stderr, FILE_NAME_EXCEED_MSG);
      exit(1);
    }

    f->file_name = file_indicator;
    f->file_content = EMPTY_FILE;
    f->size = 0;
  }

  return f;
}

int write_file(file* file_data, char* file_string) {
  char* new_file_content;
  int new_size = strlen(file_string);
  char* file_name = file_data->file_name;
  char* file_path = file_data->file_path;
  
  char* file_indicator = file_name != NULL ? file_name : file_path;
  FILE* fp;
  int status;

  fp = fopen(file_indicator, "w");
  status = fprintf(fp, "%s", file_string);
  
  fclose(fp);

  new_file_content = (char*) calloc((new_size + 1), sizeof(char));
  strcpy(new_file_content, file_string);
  
  file_data->file_content = new_file_content;
  file_data->size = new_size;
  
  return status;
}

char* read_file(file* file_data) {
  char* file_path = file_data->file_path;
  int file_size;
  char* buffer;
  
  if (file_data->file_path == NULL)
    return NULL;

  FILE* fp = fopen(file_path, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file at: %s\n", file_path);
    exit(1);
  }

  fseek(fp, 0L, SEEK_END);
  file_size = ftell(fp);
  rewind(fp);

  buffer = (char*) malloc((file_size + 1) * sizeof(char));
  *(buffer + file_size) = 0;

  fread(buffer, file_size, sizeof(char), fp);
  fclose(fp);

  return buffer;
}

void destroy_file(file* f) {
  free(f->file_content);
  free(f);
}
