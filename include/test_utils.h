#ifndef __SH_ED_TEST_UTILS__
#define __SH_ED_TEST_UTILS__

/*
 * Framework and utilities for testing.
 * Usage follows the following structure:
 
  test_info_t Test(...) {
    test_info_t test_info = InitTestInfo("TestName");

    if (condition) { // Test failed
      PrintTestInfo(test_info);
      RETURN_ERROR(TEST_FAILED);
    }

    // More tests...

    return info;
  }

 * See tests under the "tests" directory for examples.
 */

#include "utils.h"

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
test_info_t InitTestInfo(const char *test_name);
bool_t WasTestSuccessful(test_info_t info);

#endif /* __SH_ED_TEST_UTILS__ */
