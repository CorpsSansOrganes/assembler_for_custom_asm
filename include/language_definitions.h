#ifndef __LANGUAGE_DEFINITIONS__
#define __LANGUAGE_DEFINITIONS__

/*
 * @brief Definitions for reserved words and other building blocks
 * of the supported assembly language.
 */

char *reserved_instructions[] = 
  {
    "mov",
    "cmp",
    "add",
    "sub",
    "lea",
    "clr",
    "not",
    "inc",
    "dec",
    "jmp",
    "bne",
    "red",
    "prn",
    "jsr",
    "rts",
    "stop",
  };

char *reserved_directive[] =
  {
    ".data",
    ".entry",
    ".string",
    ".extern",
  };

#endif /* __LANGUAGE_DEFINITIONS__ */
