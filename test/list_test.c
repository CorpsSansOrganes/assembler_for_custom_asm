#include "list.h"
#include <stdio.h>

typedef enum {
  TEST_SUCCESSFUL,
  TEST_FAILED
} test_result_t;

test_result_t CreateListTest(void) {
  /* 
   * This is a smoke test.
   * It may also be used w/ valgrind to check for memory leaks.
  */ 

  list_t *list = CreateList();
  if (NULL == list) {
    return TEST_FAILED;
  }

  DestroyList(list);
  return TEST_SUCCESSFUL;
}

int main(void) {
  int total_failures = 0;
  if (TEST_FAILED == CreateListTest()) {
    printf("Test failed: CreateListTest\n");
    ++total_failures;
  }

  if (0 == total_failures) {
    printf("Test successful: List");
  }

  return total_failures;
}
