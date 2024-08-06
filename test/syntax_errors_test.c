#include "test_utils.h"
#include "syntax_errors.h"

test_info_t DetectExtraCharactersTest(void) {
  test_info_t test_info = InitTestInfo("DetectExtraCharacters");
  const char *no_extra_characters = "abc";
  const char *no_extra_characters_with_space = "abc  ";
  const char *extra_characters = "abc  a";
  const char *extra_characters_tab = "abc a";

  if (FALSE != DetectExtraCharacters(no_extra_characters)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}
