#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "f_utils.h"


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
    }

  return f;
}

int write_file(file* file_data, char* file_string) {
  FILE* fp;
  char* file_name = file_data->file_name;
  int status;

  fp = fopen(file_name, "w");
  status = fprintf(fp, "%s", file_string);
  
  fclose(fp);
  
  return status;
}

void destroy_file(file* f) {
  free(f);
}
