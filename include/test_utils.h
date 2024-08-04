#ifndef __TEST_UTILS__
#define __TEST_UTILS__

/*
 * Framework and utilities for testing.
 */

typedef enum {
  TEST_SUCCESSFUL,
  TEST_FAILED,
  TECHNICAL_ERROR
} test_result_t;

typedef struct {
  const char *test_name;
  test_result_t result;
  unsigned int line;
} test_info_t;

#define RETURN_ERROR(test_result) \
    do { \
        test_info.result = (test_result); \
        test_info.line = (__LINE__); \
        return test_info; \
    } while (0)

void PrintTestInfo(test_info_t info);

#endif /* __TEST_UTILS__ */
