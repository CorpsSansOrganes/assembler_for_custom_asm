#include <stdio.h> /* printf */
#include <string.h> /* strcmp */
#include "string_utils.h"
#include "test_utils.h"

test_info_t StripLeadingWhitespacesTest(void) {
    test_info_t test_info = InitTestInfo("StripLeadingWhitespacesTest");
    char leading_whitespaces_w_text[] = "   \t   Hello, World!";
    char no_leading_whitespaces[] = "NoLeadingWhitespaces";
    char only_whitespaces[] = " \t ";
    char empty_string[] = "";

    char *result;

    result = StripLeadingWhitespaces(leading_whitespaces_w_text);
    if (0 != strcmp(result, "Hello, World!")) {
        RETURN_ERROR(TEST_FAILED);
    }

    result = StripLeadingWhitespaces(no_leading_whitespaces);
    if (0 != strcmp(result, "NoLeadingWhitespaces")) {
        RETURN_ERROR(TEST_FAILED);
    }

    result = StripLeadingWhitespaces(only_whitespaces);
    if (0 != strcmp(result, "")) {
        RETURN_ERROR(TEST_FAILED);
    }

    result = StripLeadingWhitespaces(empty_string);
    if (0 != strcmp(result, "")) {
        RETURN_ERROR(TEST_FAILED);
    }

    return test_info;
}

int main() {
  int total_failures = 0;
  test_info_t test_info;
  test_info = StripLeadingWhitespacesTest();
  if (!WasTestSuccessful(test_info)) {
    PrintTestInfo(test_info);
    ++total_failures;
  }

  if (0 == total_failures) {
    printf(BOLD_GREEN "Test successful: " COLOR_RESET "string_utils\n");
  }

  return total_failures;
}
