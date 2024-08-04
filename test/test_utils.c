#include <stdio.h> /* printf */
#include "test_utils.h"

void PrintTestInfo(test_info_t info) {
  switch (info.result) {
    case TEST_SUCCESSFUL:
      printf("Test '%s' was successful\n", info.test_name);
      break;

    case TEST_FAILED:
      printf("Test '%s' was failed in line %d\n", info.test_name, info.line);
      break;

    case TECHNICAL_ERROR:
      printf("Test '%s' resulted in a technical error in line %d\n",
             info.test_name, info.line);
      break;
  }
}

test_info_t InitTestInfo(const char *test_name) {
  test_info_t info;
  info.test_name = test_name;
  info.line = 0;
  info.result = TEST_SUCCESSFUL;
  return info;
}

bool_t WasTestSuccesful(test_info_t info) {
  return (TEST_SUCCESSFUL == info.result);
}
