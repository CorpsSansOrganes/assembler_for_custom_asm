
#include "utils.h"
#include "vector.h"
#include "symbol_table.h"
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>


result_t GENERATE_OBJ_FILE (vector_t *opcode, symbol_table_t *symbol_table, char *output_path, int IC, int DC){
  FILE *obj_file = NULL;
  char *str_to_write =NULL;
  char dc [5] = sprintf (dc, "%d",DC);/* need to chos */
  obj_file = fopen(output_path, "r");
  if (NULL == obj_file) {
    perror("Couldn't open input file");
    DestroySymbolTable(symbol_table);
    return ERROR_OPENING_FILE; 
  }
  str_to_write = strcat(, str2);
  if (EOF == fputs(str_to_write, obj_file)) {
        perror("Error writing to file");
        return ERROR_WRITING_TO_FILE;
      }
  
}
