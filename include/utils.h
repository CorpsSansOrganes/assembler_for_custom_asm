#ifndef __UTILS__
#define __UTILS__

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
  ERROR_CLOSING_FILE
} result_t;

#endif /* __UTILS__ */
