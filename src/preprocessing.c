#include <stdio.h> /* perror */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcpy */
#include "preprocessing.h"
#include "syntax_errors.h"
#include "print_errors.h"
#include "string_utils.h"

static bool_t IsComment(const char *line);
static bool_t IsBlankLine(const char *line);
static bool_t IsNewMacro(const char *line);
static result_t ReadMacro(FILE *file,
                          macro_table_t *table,
                          unsigned int *line_number);

/* ~~--~~--~~--~~--~~
  Preprocessor
  ~~--~~--~~--~~--~~ */
macro_table_t *PreprocessFile(char *input_path, char *output_path) {
  bool_t should_write_line = TRUE;
  bool_t no_error_occurred = TRUE;
  unsigned int line_number = 1;
  int total_errors = 0;
  char line[MAX_LINE_SIZE];
  FILE *input_file = NULL;
  FILE *output_file = NULL;
  result_t res = SUCCESS;
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
    char *clean_line = StripWhitespaces(line);
    const char *str_to_write = NULL;

    /* Analyse what type of line that is */
    if (IsComment(clean_line) | IsBlankLine(clean_line)) {
      should_write_line = FALSE;
    }

    else if (IsNewMacro(clean_line)) {
      if (SUCCESS != ReadMacro(input_file, table, &line_number)) {
        no_error_occurred = FALSE;
      }
    }

    else {
      macro_t *macro = FindMacro(table, clean_line);

      if (NULL != macro) { /* Macro usage */
        str_to_write = GetMacroDefinition(macro);
      } else {
        str_to_write = clean_line;
      }
    }

    /* Write to output file */
    if (no_error_occurred && should_write_line) {
    }
  }

  return table;
}

/* ~~--~~--~~--~~--~~
  Static functions
  ~~--~~--~~--~~--~~ */
/*
 * @brief Checks if a line is a comment.
 * 
 * @param line - Pointer to a null-terminated string containing the line to be
 *               checked.
 * 
 * @return TRUE if the line is a comment, FALSE otherwise.
 */

static bool_t IsComment(const char *line) {
  return IsPrefix(StripLeadingWhitespaces(line), ";");
}

static bool_t IsNewMacro(const char *line) {
  return IsPrefix(StripLeadingWhitespaces(line), "macr");
}

/*
 * @brief Reads a macro definition from a file and adds it to a macro table.
 * 
 * This function processes a macro definition starting from the current file 
 * pointer. 
 * 'line number' is incremented and 'file' is updated to point to the next line
 * after the macro definition.
 *
 * Syntax errors are reported to STDERR, and the function 
 * returns a status code indicating the result of the operation.
 * 
 * @param file - Pointer to a FILE object positioned at the start of a macro
 *               definition.
 *               On successful completion, this will point to the line immediately
 *               following the 'endmacr' directive of the macro definition.
 *
 *        table - Pointer to a macro_table_t structure where the macro will be
 *                added if the macro definition is valid.
 *
 *        line_number - Pointer to an unsigned int representing the current line
 *                      number. 
 *                      This will be incremented to reflect the number of lines
 *                      read from the file.
 * 
 * @return SUCCESS if the macro was successfully read and added to the macro table 
 *         with no syntax errors.
 *
 *         FAILURE if a syntax error was encountered while reading the macro definition.
 *
 *         MEM_ALLOCATION_ERROR if the macro definition was valid but a memory 
 *         allocation error occurred while adding the macro to the table.
 */

static result_t ReadMacro(FILE *file,
                          macro_table_t *table,
                          unsigned int *line_number) {

  /*
   * Syntax errors to check:
   * 1. No extra characters at the end of macr or endmacr line.
   * 2. Macro name isn't a reserved name.
   */
  return SUCCESS; // TODO
}
