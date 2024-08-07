#include <string.h> /* strncmp */
#include "string_utils.h"

bool_t IsPrefix(const char *str, const char *prefix) {
  return (0 == strncmp(str, prefix, strlen(prefix)));
}
