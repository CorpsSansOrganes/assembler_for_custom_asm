#include <string.h> /* strlen, strcmp */
#include "linting.h"
#include "string_utils.h"

char *CleanLine(char *line) {
  char *src = StripWhitespaces(line);
  char *dest = src;
  char *last_non_blank = NULL;
  bool_t in_whitespace = FALSE;
  bool_t inside_string = FALSE;

  if (IsBlankLine(line)) {
    return src;
  }

  /* Collapse multiple whitespaces between words */
  while ('\n' != *src) {
    if (*src == '"'){
      if (FALSE == inside_string){
        inside_string = TRUE;
      }
    }
    /* Copy characters from src to dest if non-blanks */
    if (!IsBlank(*src)) {
      *dest = *src;
      last_non_blank = dest;
      ++dest;
      in_whitespace = FALSE;
    }

    /* Copy just one space when in a sequence of whitespaces */
    else if (FALSE == in_whitespace && FALSE == inside_string) {
      *dest = ' ';
      ++dest;
      in_whitespace = TRUE;
    }
    ++src;
  }

  /* Restore terminating characters */
  *(last_non_blank + 1) = '\n';
  *(last_non_blank + 2) = '\0';
  
  return line;
}

bool_t IsBlankLine(const char *line) {
  if (0 != strcmp(line, "\n")) {
    return FALSE;
  }

  return TRUE;
}
