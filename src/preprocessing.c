#include <stdio.h> /* perror */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcpy */
#include "preprocessing.h"
#include "syntax_errors.h"
#include "string_utils.h"

static bool_t IsComment(const char *line);
static bool_t IsNewMacro(const char *line);
static syntax_error_code_t ReadMacro(FILE *file, macro_table_t *table);

/* ~~--~~--~~--~~--~~
  Preprocessor
  ~~--~~--~~--~~--~~ */
macro_table_t *PreprocessFile(char *input_path, char *output_path) {
  bool_t should_write_line = TRUE;
  int line_number = 1;
  int total_errors = 0;
  char line[MAX_LINE_SIZE];
  FILE *input_file = NULL;
  FILE *output_file = NULL;
  syntax_error_code_t result = VALID_SYNTAX;
  macro_table_t *table = CreateMacroTable();

  if (NULL == table) {
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a macro table\n");
    return NULL;
  }

  input_file = fopen(input_path, "r");
  if (NULL == input_file) {
    perror("Couldn't open input file");
    DestroyMacroTable(table);
    return NULL;
  }

  output_file = fopen(output_path, "w");
  if (NULL == output_file) {
    perror("Couldn't open output file");
    fclose(input_file);
    DestroyMacroTable(table);
    return NULL;
  }

  while (NULL != fgets(line, MAX_LINE_SIZE, input_file)) {
    if(IsComment(line)) {
      should_write_line = FALSE;
    }
    else if (IsNewMacro(line)) {
      result = ReadMacro(input_file, table);
      if ( ReadMacro(input_file, table)) {
      }
    }
  }

  /*
  Open file.
  For each line in file:
    If (IsNewMacro(line)):
        1. Check that name isn't reserved.
        2. Create new entry (AddMacroIfUnique(macros, line))
        3. If error occured, inc. total_errors.
        4. Skip to the end of the macro macro_definition.
      
    Else 
      to_Write = line
      If (IsMacro(line)):
            1. Find macro name in the macros.
            2. to_write = macro_definition for the macro
          
      Write to_write to buffer / new_file
  Write a new file.
  */

  /*
   * Syntax errors to check:
   * 1. No extra characters at the end of macr or endmacr line.
   * 2. Macro name isn't a reserved name.
   */
  return table;
}
