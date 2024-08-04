#include <stdio.h> /* fprintf, printf, sprintf */
#include "linting.h"
#include "test_utils.h"

#define BUFFER_SIZE 256
test_info_t TestLinting(const char *input_file) {
  char test_name[BUFFER_SIZE];
  char output_name[BUFFER_SIZE];
  sprintf(test_name, "TestLinting for file %s", input_file);
  sprintf(output_name, "%s_linted", input_file);
  test_info_t test_info = InitTestInfo(test_name);

  if (SUCCESS != LintFile(input_file, output_name)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

int main(int argc, char *argv[]) {
  int i = 0;
  int error_count = 0;
  if (argc <= 1) {
    fprintf(stderr, "Usage: %s <file1> [file2 ... fileN]\n", argv[0]);
    return 1;
  }

  for (i = 1; i < argc; ++i) {
    const char *input_file = argv[i];
    test_info_t info = TestLinting(input_file);
    if (!WasTestSuccessful(info)) {
      PrintTestInfo(info);
      ++error_count;
    }
  }

  if (0 == error_count) {
    printf("Test successful: Linting");
  }

  return error_count;
}
