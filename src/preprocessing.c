#include <stdio.h> /* perror */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcpy */
#include <ctype.h> /* isblank */
#include "preprocessing.h"
#include "syntax_errors.h"
#include "print_errors.h"
#include "string_utils.h"

static bool_t IsComment(const char *line);
static bool_t IsBlankLine(const char *line);
static bool_t IsNewMacro(const char *line);
static result_t ParseMacro(FILE *file,
                           macro_table_t *table,
                           char *buffer,
                           unsigned int *line_number);

/* ~~--~~--~~--~~--~~
  Preprocessor
  ~~--~~--~~--~~--~~ */
macro_table_t *PreprocessFile(char *input_path, char *output_path) {
  bool_t should_write_line = TRUE;
  bool_t error_occurred = FALSE;
  unsigned int line_number = 1;
  char *line = NULL;
  FILE *input_file = NULL;
  FILE *output_file = NULL;
  macro_table_t *table = CreateMacroTable();
  long pos = 0;

  /* Acquire resources */
  line = (char *)malloc(MAX_LINE_SIZE * sizeof(char));
  if (NULL == line) {
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a buffer\n");
    return NULL;
  }
  if (NULL == table) {
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a macro table\n");
    free(line);
    return NULL;
  }

  input_file = fopen(input_path, "r");
  if (NULL == input_file) {
    perror("Couldn't open input file");
    free(line);
    DestroyMacroTable(table);
    return NULL;
  }

  output_file = fopen(output_path, "w");
  if (NULL == output_file) {
    perror("Couldn't open output file");
    free(line);
    fclose(input_file);
    DestroyMacroTable(table);
    return NULL;
  }

  /*
   * First pass:
   * Parse macros, populating the macro table.
   * Check for syntax errors in macro definitions.
   */
  pos = ftell(input_file);
  if (-1 == pos) {
    perror("Error getting file position");
    free(line);
    fclose(input_file);
    DestroyMacroTable(table);
    return NULL;
  }

  while (NULL != fgets(line, MAX_LINE_SIZE, input_file)) {
    line = StripWhitespaces(line);

    if (IsNewMacro(line)) {
      if (fseek(input_file, pos, SEEK_SET)) {
        perror("Error changing file position");
        error_occurred = TRUE;
        break;
      }
      if (SUCCESS != ParseMacro(input_file, table, line, &line_number)) {
        error_occurred = TRUE;
      }
    }

    pos = ftell(input_file);
    if (-1 == pos) {
      perror("Error getting file position");
      error_occurred = TRUE;
      break;
    }

    ++line_number;
  }

  if (error_occurred) {
    free(line);
    fclose(input_file);
    fclose(output_file);
    DestroyMacroTable(table);
    return NULL;
  }

  /*
   * Second pass:
   * Since no errors occurred, write to file.
   * Expand macros to their definitions.
   */
  
  fseek(input_file, 0, SEEK_SET);

  while (NULL != fgets(line, MAX_LINE_SIZE, input_file)) {
    line = StripWhitespaces(line);
    const char *str_to_write = NULL;

    if (IsNewMacro(line)) {
      while(FALSE == IsPrefix(line, "endmacr")) {
        fgets(line, MAX_LINE_SIZE, input_file); /* Assuming every macro has endmacr! */
        line = (char *)StripLeadingWhitespaces(line);
      }
      line = StripTrailingWhitespaces(line);
    }

    else if (IsComment(line) | IsBlankLine(line)) {
      should_write_line = FALSE;
    }


    else {
      macro_t *macro = FindMacro(table, line);

      if (NULL != macro) { /* Macro usage */
        str_to_write = GetMacroDefinition(macro);
      } else {
        str_to_write = line;
      }
    }

    /* Write to output file */
    if (should_write_line) {
      if (EOF == fputs(str_to_write, output_file)) {
        perror("Error writing to file");
      }
    }
  }

  free(line);
  fclose(input_file);
  fclose(output_file);
  DestroyMacroTable(table);
  return NULL;

  return table;
}

/* ~~--~~--~~--~~--~~
  Static functions
  ~~--~~--~~--~~--~~ */
/*
 * @brief Checks if a line is a comment.
 *        We assume there are no leading white spaces (spaces or tabs).
 * 
 * @param line - Pointer to a null-terminated string containing the line to be
 *               checked.
 * 
 * @return TRUE if the line is a comment, FALSE otherwise.
 */

static bool_t IsComment(const char *line) {
  return IsPrefix(line, ";");
}

/*
 * @brief Checks if a line is blank (containing only white spaces, or no
 *        characters at all).
 * 
 * @param line - Pointer to a null-terminated string containing the line to be
 *               checked.
 * 
 * @return TRUE if the line is blank, FALSE otherwise.
 */

static bool_t IsBlankLine(const char *line) {
  char *ptr = (char *)StripLeadingWhitespaces(line);
  while (isblank(*ptr)) {
    ++ptr;
  }

  if ('\0' == *ptr) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/*
 * @brief Checks if a line is a macro.
 *        We assume there are no leading white spaces (spaces or tabs).
 * 
 * @param line - Pointer to a null-terminated string containing the line to be
 *               checked.
 * 
 * @return TRUE if the line is a macro, FALSE otherwise.
 */

static bool_t IsNewMacro(const char *line) {
  return IsPrefix(line, "macr ");
}

/*
 * @brief Reads a macro definition from a file and adds it to a macro table.
 * 
 * This function processes a macro definition starting from the current file 
 * pointer. 
 * 'line number' is incremented and 'file' is updated to point to the next line
 * after the macro definition.
 *
 * We macros are defined in the format:
 * macr m_name
 * ...
 * endmacr
 *
 *
 * Syntax errors are reported to STDOUT, and the function returns a status code
 * indicating the result of the operation.
 * 
 * Syntax errors checked are:
 * 1. No extra character after macro guards. e.g.:
 *    "macr m_name" is valid
 *    "macr m_name  axc" is illegal.
 *    "endmacrqwe" is illegal.
 * 2. Macro name isn't a reserved name by the language.
 * 
 * @param file - Pointer to a FILE object positioned at the start of a macro
 *               definition. e.g.:
 *               macr m_name <-- file
 *               ...
 *               endmacr
 *
 *               On successful completion, this will point to the line immediately
 *               following the 'endmacr' directive of the macro definition.
 *
 *        table - Pointer to a macro_table_t structure where the macro will be
 *                added if the macro definition is valid.
 *
 *        line - Buffer in which lines read by the function will be stored.
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
 *
 *         FAIL_HANDLING_ERROR if there was an error with the file, e.g. reading 
 *         from it.
 */

static result_t ParseMacro(FILE *file,
                           macro_table_t *table,
                           char *line,
                           unsigned int *line_number) {
  /*
   * Syntax errors to check:
   * 1. No extra characters at the end of macr or endmacr line.
   * 2. Macro name isn't a reserved name.
   */

  /* Reading macro name */ 
  if (NULL == fgets(line, MAX_LINE_SIZE, file)) {
    perror("Error reading from file");
    return FILE_HANDLING_ERROR;
  }
  
  /* Assuming every macro has endmacr! */
  while(FALSE == IsPrefix(line, "endmacr")) {
    if (NULL == fgets(line, MAX_LINE_SIZE, file)) {
      perror("Error reading from file");
      return FILE_HANDLING_ERROR;
    }
      line = (char *)StripLeadingWhitespaces(line);
    }

  return SUCCESS; // TODO
}
