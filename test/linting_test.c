#include <stdio.h> /* printf */
#include "linting.h"

int main(void) {
  const char *input_path = "./test_files/linting_test.txt";
  const char *output_path = "./test_files/linting_test_result.txt";
  if (SUCCESS != LintFile(input_path, output_path)) {
    printf("LintFile wasn't succesful");
    return 1;
  }

  return 0;
}
