#include <string.h> /* strncmp, strlen */
#include <stdlib.h> /* malloc, free */
#include "string_utils.h"

bool_t IsPrefix(const char *str, const char *prefix) {
  return (0 == strncmp(str, prefix, strlen(prefix)));
}

char *StripWhitespaces(char *str) {
  return StripTrailingWhitespaces((char *)StripLeadingWhitespaces(str));
}

char *StripLeadingWhitespaces(char *str) {
  char *to = str;
  size_t length = 0;

  while (IsBlank(*str)) {
    ++str;
  }

  /* No leading whitespaces */
  if (str == to) {
    return str;
  }

  length = strlen(str) + 1;
  memmove(to, str, length);

  return str;
}

char *StripTrailingWhitespaces(char *str) {
  char *end = NULL;

  if ('\0' == *str) {
    return str;
  }

  end = (char *)EndOfString(str) - 1;
  while (end > str && IsBlank(*end)) {
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
