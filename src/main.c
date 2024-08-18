/* main.c
 *
 * The main module that processes command-line arguments and initiates the assembling process.
 */

#include <stdio.h> /* fopen, close */
#include <stdlib.h>
#include <unistd.h>
#include "macro_table.h"
#include "utils.h"
#include "assembler.h"
#include "preprocessing.h"

static const char *ProduceFilePath(const char *dir_path,
                                   const char *file_name,
                                   const char *extension,
                                   char *full_path);

int main(int argc, char *argv[]) {
  char input_path[200];
  char assembler_input_path[200];
  char directory[150];
  macro_table_t *macro_table = NULL;
  int i = 0;
  bool_t assembling_error = FALSE;

  if (NULL == getcwd(directory, sizeof(directory))) {
    perror ("Error getting the current working directory path");
    return 1;
  }

  printf("dir: %s\n", directory);

  /* Handle no arguments passed */
  if (1 >= argc) {
    fprintf(stderr, "Usage: %s file_name1 [...]\n", argv[0]);
    return 1;
  }

  /* For each input file, run the assembler */
  for (i = 1; i < argc; ++i) {
    int total_failures = 0; 
    ProduceFilePath(directory, argv[i], ".as", input_path);
    ProduceFilePath(directory, argv[i], ".am", assembler_input_path);

    /* Run preprocessing */
    macro_table = PreprocessFile(input_path, assembler_input_path);
    if (NULL == macro_table) {
      total_failures++;
      assembling_error = TRUE;
    }

    /* Run assembler */
    else if (SUCCESS != AssembleFile(assembler_input_path,macro_table)) {
      total_failures++;
      assembling_error = TRUE;
    }

    if (0 == total_failures) {
      printf(BOLD_GREEN "Assembler successfully finished" COLOR_RESET " for %s\n", argv[i]);
    }
    else {
      printf(BOLD_RED "Assmbler error" COLOR_RESET " for %s\n", argv[i]);
    }
  }
  return assembling_error;
}

static const char *ProduceFilePath(const char *dir_path,
                                   const char *file_name,
                                   const char *extension,
                                   char *full_path) {
    sprintf(full_path, "%s/%s%s", dir_path, file_name, extension);
    return full_path;
}
