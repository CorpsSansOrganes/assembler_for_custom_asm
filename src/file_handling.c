#include <stdio.h> /* fopen, fclose, fread, fwrite */
#include <stdlib.h> /* perror, malloc */
#include "file_handling.h"

char *FileToString(const char *file_path) {
  FILE *file = fopen(file_path, "r");
  size_t file_size = 0;
  char *string = NULL;

  if (NULL == file) {
    perror("Failed to open file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (-1 == file_size) {
    perror("Couldn't determine file size");
    return NULL;
  }

  string = (char *)malloc(file_size + 1);
  if (NULL == string) {
    perror("Couldn't allocate memory");
    fclose(file);
    return NULL;
  }

  if (file_size != fread(string, sizeof(char), file_size, file)) {
    perror("Couldn't read the entire content of the file");
    free(string);
    fclose(file);
    return NULL;
  }

  string[file_size] = '\0';
  if (fclose(file)) {
    perror("Couldn't properly close the file");
  }
  return string;
}

result_t StringToFile(char *file_path, char *string, size_t length) {
  FILE *file = fopen(file_path, "w");

  if (NULL == file) {
    perror("Failed to open file");
    return ERROR_OPENING_FILE;
  }

  if (length != fwrite(string, sizeof(char), length, file)) {
    perror("Couldn't write entire string to file");
    fclose(file);
    return ERROR_WRITING_TO_FILE;
  }

  if (fclose(file)) {
    perror("Couldn't properly close the file");
    return ERROR_CLOSING_FILE;
  }
  return SUCCESS;
}
