#ifndef __UTILS__
#define __UTILS__

#define  blank_delimiters " \t\n\r"

typedef enum {
  FALSE,
  TRUE
} bool_t;

typedef enum {
  SUCCESS,
  FAILURE,
  MEM_ALLOCATION_ERROR,
  ERROR_OPENING_FILE,
  ERROR_WRITING_TO_FILE,
  ERROR_CLOSING_FILE,
  FILE_HANDLING_ERROR
} result_t;

#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"

#endif /* __UTILS__ */
