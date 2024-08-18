/* main.c
 *
 * The main module that processes command-line arguments and initiates the assembling process.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "macro_table.h"
#include "utils.h"
#include "assembler.h"
#include "preprocessing.h"

static const char *ProduceFilePath(const char *dir_path,
                                   const char *file_name,
                                   const char *extension,
                                   char *full_path);

int main(int argc, char *argv[]) {
   char *input_path[256];
   char *assembler_input_path[256];
   char *directory [1024];
   macro_table_t *macro_table; 
   int total_failures = 0; 

   if (NULL == getcwd (directory, sizeof(directory))){
        perror ("error in getcwd");
        return 1;
   }
   strcat (directory, "./");
   while (--argc > 0) {
      ProduceFilePath(directory, argv[argc], ".as", input_path);
      ProduceFilePath(directory, argv[argc], ".am", assembler_input_path);
      macro_table = PreprocessFile (input_path,assembler_input_path);
      if (NULL == macro_table){
        perror ("error in preprocessing");
        total_failures++;
        continue;
      }
    if (SUCCESS != AssembleFile(assembler_input_path,macro_table)){
        perror ("error in processing");
        total_failures++;
        continue;
    }
    }
return total_failures;
}


static const char *ProduceFilePath(const char *dir_path,
                                   const char *file_name,
                                   const char *extension,
                                   char *full_path) {
    sprintf(full_path, "%s/%s%s", dir_path, file_name, extension);
    return full_path;
}
