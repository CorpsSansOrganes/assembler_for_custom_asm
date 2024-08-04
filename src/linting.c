#include <stdio.h> /* perror */
#include "linting.h"
#include "utils.h"
#include "file_handling.h"

result_t LintFile(char *input_path, char *output_path) {
  char *file_content = FileToString(input_path);
  if (NULL == file_content) {
    perror("Couldn't read input file to string");

  }
  /*
  1. Open file.
  2. For each line in the file:
    1. Check if its a comment - if it is, remove it.
    2. Check if its a blank line - if it is, remove it.
    3. Else
  */
}

/*
 * @brief Detects if a line is a comment.
 *   According to the specified standard, a line which
 *   starts with ';' is a comment.
 *
 * @param line - The line to check.
 * @return TRUE if line is a comment, FALSE otherwise.
 */
static int IsLineAComment(char *line);

/*
 * @brief Detects if a given line is an empty statement.
 *   Line is considered an empty statement (or a blank line) according to
 *   the standard if it includes only the characters ' ' or '\t'.
 *
 * @param line - The line to check.
 * @return TRUE if line is a blank line, FALSE otherwise.
 */
static int IsLineBlank(char *line) {
  // Use remove extranous spaces?
  return 0; // TODO
}

static void RemoveExtraneousSpaces(char *line) {
  // Blank characters = ' ', '\t'
  // 1. While char is blank character, remove it. (Remove all blanks from the start).
  // 2. If more than 1 blank occurs:
  //     Remove excess blanks, until only 1 is left.
  // 3. Check if last one is blank, if it is remove it.
}
