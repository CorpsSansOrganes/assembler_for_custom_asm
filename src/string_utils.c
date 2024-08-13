#include <string.h> /* strncmp, strlen */
#include <stdlib.h> /* malloc, free */
#include "string_utils.h"

bool_t IsPrefix(const char *str, const char *prefix) {
  return (0 == strncmp(str, prefix, strlen(prefix)));
}

char *StripWhitespaces(char *str) {
  return StripTrailingWhitespaces((char *)StripLeadingWhitespaces(str));
}

const char *StripLeadingWhitespaces(const char *str) {
  while (IsBlank(*str)) {
    ++str;
  }

  return str;
}

char *StripTrailingWhitespaces(char *str) {
  char *end = (char *)EndOfString(str) - 1;
  while (IsBlank(*end) && end > str) {
    --end;
  }

  end[1] = '\0';
  return str;
}

const char *EndOfString(const char *str) {
  return str + strlen(str);
}

char *CopySubstring(const char *from, const char *to, char *dest) {
  size_t length = to - from;

  strncpy(dest, from, length);
  dest[length] = '\0';
  return dest;
}

char *StrDup(const char *str) {
  size_t len = strlen(str);
  char *dup = (char *)malloc(len + 1);

  if (NULL == dup) {
    return NULL; 
  }

  return CopySubstring(str, str + len, dup);
}

int IsBlank(int c) {
  return ((' ' == c) || ('\t' == c));
}
