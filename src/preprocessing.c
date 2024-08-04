#include <stdio.h> /* perror */
#include "preprocessing.h"
#include "syntax_errors.h"
#include "file_handling.h"

typedef struct {
  char *name;
  void *address;
} macro_t;

result_t PreprocessFile(char *file_path) {
  int total_errors = 0;
  list_t *macros = CreateList();

  char *file_content = FileToString(file_path);
  if (NULL == file_content) {
    perror("Error reading file content");
  }

  /*
  Open file.
  For each line in file:
    If (IsNewMacro(line)):
        1. Check that name isn't reserved.
        2. Create new entry (AddMacro(macros, line))
        3. If error occured, inc. total_errors.
        4. Skip to the end of the macro definition.
      
    Else 
      to_Write = line
      If (IsMacro(line)):
            1. Find macro name in the macros.
            2. to_write = definition for the macro
          
      Write to_write to buffer / new_file
  Write a new file.
  */

  /*
   * Syntax errors to check:
   * 1. No extra characters at the end of macr or endmacr line.
   * 2. Macro name isn't a reserved name.
   */
  
  return total_errors;
}

static int IsNewMacro(char *line) {
  return 0; // TODO
}

/*
 * Registers a new macro into the macro table.
 *
 * @return 0 upon success, or an error code.
 */
static int AddMacro(list_t *macros, char *line) {
  return 0; // TODO
}

static int CreateOutputFile(char *str) {
  return 0; // TODO
}
