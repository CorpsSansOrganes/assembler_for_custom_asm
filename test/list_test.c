#include "list.h"
#include <stdio.h>
#include <stdlib.h>

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

static void PrintTestInfo(test_info_t info) {
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

test_info_t CreateListTest(void) {
  /* 
   * This is a smoke test.
   * It may also be used w/ valgrind to check for memory leaks.
  */ 

  test_info_t test_info = {"CreateListTest", TEST_SUCCESSFUL, 0};

  list_t *list = CreateList();
  if (NULL == list) {
    RETURN_ERROR(TEST_FAILED);
  }

  DestroyList(list);
  return test_info;
}

test_info_t AddNodeTest(void *value) {
  test_info_t test_info = {"AddNodeTest", TEST_SUCCESSFUL, 0};
  list_t *list = CreateList();
  node_t *node = NULL;
  node_t *next_node = NULL;

  if (NULL == list) {
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  node = AddNode(list, value);
  if (NULL == node) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (GetValue(node) != value) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (NULL != GetNext(node)) {
    RETURN_ERROR(TEST_FAILED);
  }

  next_node = AddNode(list, value);
  if (NULL == next_node) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (GetValue(next_node) != value) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (next_node != GetNext(node)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (NULL != GetNext(next_node)) {
    RETURN_ERROR(TEST_FAILED);
  }

  DestroyList(list);
  return test_info;
}
static int SizeTCompare(void *a, void *b) {
    return ((size_t )a == (size_t)b);
}

test_info_t FindTest(void) {
  test_info_t test_info = {"FindTest", TEST_SUCCESSFUL, 0};
  list_t *list = CreateList();
  size_t i = 0;
  node_t *node = NULL;

  for (i = 1; i < 11; i++) {
    AddNode(list, (void *)(i));
  }

  node = Find(list, SizeTCompare, (void *)3);
  if (NULL == node) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (GetValue(node) != (void *)3) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (GetValue(GetNext(node)) != (void *)4) {
    RETURN_ERROR(TEST_FAILED);
  }

  node = Find(list, SizeTCompare, (void *)42);
  if (NULL != node) {
    RETURN_ERROR(TEST_FAILED);
  }

  DestroyList(list);
  return test_info;
}

test_info_t GetNextTest(void) {
  test_info_t test_info = {"FindTest", TEST_SUCCESSFUL, 0};
  list_t *list = CreateList();
  size_t i = 0;
  node_t *node = NULL;

  for (i = 1; i < 11; i++) {
    AddNode(list, (void *)(i));
  }

  i = 1;
  node = Find(list, SizeTCompare, (void *)1);
  while (NULL != node) {
    if (!SizeTCompare(GetValue(node), (void *)i)) {
      RETURN_ERROR(TEST_FAILED);
    }
    ++i;
    node = GetNext(node);
  }

  if (10 != i) {
    RETURN_ERROR(TEST_FAILED);
  }

  DestroyList(list);
  return test_info;
}

int main(void) {
  int total_failures = 0;
  test_info_t test_info; 

  test_info = CreateListTest();
  if (TEST_SUCCESSFUL != test_info.result) {
    PrintTestInfo(test_info);
    ++total_failures;
  }

  test_info = AddNodeTest((void *)42);
  if (TEST_SUCCESSFUL != test_info.result) {
    PrintTestInfo(test_info);
    ++total_failures;
  }

  test_info = FindTest();
  if (TEST_SUCCESSFUL != test_info.result) {
    PrintTestInfo(test_info);
    ++total_failures;
  }

  if (0 == total_failures) {
    printf("Test successful: List\n");
  }

  return total_failures;
}
