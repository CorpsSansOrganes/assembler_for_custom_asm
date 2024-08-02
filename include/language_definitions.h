#ifndef __LANGUAGE_DEFINITIONS__
#define __LANGUAGE_DEFINITIONS__

/*
 * @brief Definitions for reserved words and other building blocks
 * of the supported assembly language.
 */

typedef enum {
  SOURCE_OPERAND,
  DESTINATION_OPERAND,
} operand_type_t;

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
  char *register_names[] =
  {
    "r0",
    "r1",
    "r2",
    "r3",
    "r4",
    "r5",
    "r6",
    "r7",
  };

#endif /* __LANGUAGE_DEFINITIONS__ */
