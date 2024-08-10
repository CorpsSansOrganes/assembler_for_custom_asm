#include <stdio.h> /* perror */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcpy */
#include <ctype.h> /* isblank */
#include "preprocessing.h"
#include "syntax_errors.h"
#include "string_utils.h"

static bool_t IsComment(const char *line);
static bool_t IsBlankLine(const char *line);
static bool_t IsNewMacro(const char *line);
static result_t ParseMacro(FILE *file,
                           macro_table_t *table,
                           char *line,
                           unsigned int *line_number);
static result_t ReadMacrosInFile(FILE *file,
                                 macro_table_t *table,
                                 char *buffer,
                                 bool_t *error_occurred);
static result_t PerformPreprocessing(FILE *input_file,
                                   FILE *output_file,
                                   macro_table_t *table,
                                   char *buffer);

/* ~~--~~--~~--~~--~~
  Preprocessor
  ~~--~~--~~--~~--~~ */
macro_table_t *PreprocessFile(char *input_path, char *output_path) {
  bool_t error_occurred = FALSE;
  char *line = NULL;
  FILE *input_file = NULL;
  FILE *output_file = NULL;
  macro_table_t *table = CreateMacroTable();
  long pos = 0;
  result_t res;

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
  if (FILE_HANDLING_ERROR == 
    ReadMacrosInFile(input_file, table, line, &error_occurred)) {
      perror("Error parsing file to macros");
      error_occurred = TRUE;
  }

  /*
   * Second pass:
   * Since no errors occurred, write to file.
   * Expand macros to their definitions.
   */
  if (FALSE == error_occurred) {
    if (fseek(input_file, 0, SEEK_SET)) {
      perror("Error changing file position");
    }
    else {
      PerformPreprocessing(input_file, output_file, table, line);
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


/**
 * @brief Reads macros from a given file and populates the macro table.
 *
 * This function reads through the provided file line by line, using the 
 * provided buffer, and adds any detected macros to the specified macro table.
 * The buffer must have a size of at least MAX_LINE_SIZE.
 *
 * The function analyzes the entire file and, upon completion, returns whether
 * the operation was successful.
 * Additionally, it sets the `error_occurred` flag to indicate if any syntax 
 * errors were encountered during the file analysis.
 *
 * @param file - A valid pointer to a file object to be read. This file should 
 *             be opened for reading prior to calling this function.
 * @param table - A pointer to the macro table where detected macros will be added.
 * @param buffer - A pointer to a pre-allocated buffer with a size of at least 
 *                 MAX_LINE_SIZE used for reading lines from the file.
 * @param error_occurred - A pointer to a boolean flag that will be set to TRUE 
 *                       if any syntax errors occur during the file analysis; 
 *                       otherwise, it will be set to FALSE.
 * 
 * @return Returns SUCCESS if the entire file was analyzed.
 *         If an error occurred while handling the file, FILE_HANDLING_ERROR is returned.
 */

static result_t ReadMacrosInFile(FILE *file,
                                 macro_table_t *table,
                                 char *buffer,
                                 bool_t *error_occurred) {
  long pos = ftell(file);
  unsigned int line_number = 1;

  if (-1 == pos) {
    perror("Error getting file position");
    return FILE_HANDLING_ERROR;
  }

  while (NULL != fgets(buffer, MAX_LINE_SIZE, file)) {
    buffer = StripWhitespaces(buffer);

    if (IsNewMacro(buffer)) {
      if (fseek(file, pos, SEEK_SET)) {
        perror("Error changing file position");
        return FILE_HANDLING_ERROR;
      }
      if (SUCCESS != ParseMacro(file, table, buffer, &line_number)) {
        *error_occurred = TRUE;
      }
    }

    pos = ftell(file);
    if (-1 == pos) {
      perror("Error getting file position");
      return FAILURE;
    }
    ++line_number;
  }

  return SUCCESS;
}

/*
 * @brief Reads a macro definition from a file and adds it to a macro table.
 * 
 * This function processes a macro definition starting from the current file 
 * pointer. 
 * 'file' is updated to point to the next line after the macro definition, and
 * 'line_number' would be update accordingly.
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
 *        line_number - Pointer to the line number of the first line of the macro.
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

  char *macro_name_start = NULL;
  char *macro_name_end = NULL;

  /* Reading macro name */ 
  if (NULL == fgets(line, MAX_LINE_SIZE, file)) {
    perror("Error reading from file");
    return FILE_HANDLING_ERROR;
  }

  line = (char *)StripLeadingWhitespaces(line);
  macro_name_start = line + 5; /* strlen("macr "); */
  macro_name_start = (char *)StripLeadingWhitespaces(macro_name_start);
  macro_name_end = macro_name_start;
  while (!isblank(*macro_name_end)) {
    ++macro_name_end;
  }
  if ()
  
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

/**
 * @brief Performs preprocessing on the input file and writes the result to the output file.
 *
 * This function processes the input file by removing blank lines, removing comments, 
 * and expanding any macros based on their definitions in the provided macro table. 
 * The processed output is written to the specified output file.
 *
 * @param input_file - A pointer to the file to be preprocessed.
 * @param output_file - A pointer to the file where the preprocessed output will be written.
 * @param table - A pointer to the macro table containing macro definitions for expansion.
 * @param line - A pointer to a buffer used for reading and processing lines.
 *
 * @return Returns SUCCESS if preprocessing and writing to the output file were successful. 
 *         Returns ERROR_WRITING_TO_FILE if an error occurred while writing to the output file.
 */

static result_t PerformPreprocessing(FILE *input_file,
                                   FILE *output_file,
                                   macro_table_t *table,
                                   char *line) {

  bool_t should_write_line = TRUE;

  while (NULL != fgets(line, MAX_LINE_SIZE, input_file)) {
    line = StripWhitespaces(line);
    const char *str_to_write = NULL;

    /* Skip macro definitions */
    if (IsNewMacro(line)) {
      while(FALSE == IsPrefix(line, "endmacr")) {
        /* Assuming every macro has endmacr! */
        fgets(line, MAX_LINE_SIZE, input_file); 
        line = (char *)StripLeadingWhitespaces(line);
      }
      line = StripTrailingWhitespaces(line);
    }

    else if (IsComment(line) | IsBlankLine(line)) {
      should_write_line = FALSE;
    }

    /* Either macro usage or a line of code */
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
        return ERROR_WRITING_TO_FILE;
      }
    }
  }

  return SUCCESS;
}

