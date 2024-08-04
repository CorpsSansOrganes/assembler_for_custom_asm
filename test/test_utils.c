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
