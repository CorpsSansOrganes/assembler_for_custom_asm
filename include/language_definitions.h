#ifndef __LANGUAGE_DEFINITIONS__
#define __LANGUAGE_DEFINITIONS__
#include "utils.h"
#define NUM_OF_INSTRUCTIONS 16
#define NUM_OF_DIRECTIVES 4
#define NUM_OF_REGISTERS 8

/*
 * @brief Definitions for reserved words and other building blocks
 * of the supported assembly language.
 */

typedef enum {
  SOURCE_OPERAND,
  DESTINATION_OPERAND
} operand_type_t;

typedef enum {
  IMMEDIATE,
  DIRECT,
  INDIRECT_REGISTER,
  DIRECT_REGISTER,
  INVALID
} addressing_methods;

/*
* @brief struct that holds the valid addressing method for a given instruction.
*
* @param source_operand_methods - the arrays holds the valid methods for source operand. 
* @param destination_operand_methods - the arrays holds the valid methods for destination operand. 
*
* if there is no operand, all the cells hold the value false, 
* and the bool_t for the operand is also false.
*/
typedef struct {
    char *name;
    bool_t source_operand_methods[4];     
    bool_t destination_operand_methods[4];
    bool_t is_there_source_operand;
    bool_t is_there_destination_operand;
} address_method_table_t;

/*
*table of the valid address method for each instruction 
*/
address_method_table_t address_method_table [NUM_OF_INSTRUCTIONS] = 
  {{"mov", {TRUE,TRUE,TRUE,TRUE},{FALSE,TRUE,TRUE,TRUE},TRUE,TRUE},
  {"cmp", {TRUE,TRUE,TRUE,TRUE},{TRUE,TRUE,TRUE,TRUE},TRUE,TRUE},
  {"add", {TRUE,TRUE,TRUE,TRUE},{TRUE,TRUE,TRUE,TRUE},TRUE,TRUE},
  {"sub", {TRUE,TRUE,TRUE,TRUE},{FALSE,TRUE,TRUE,TRUE},TRUE,TRUE},
  {"lea", {FALSE,TRUE,FALSE,FALSE},{FALSE,TRUE,TRUE,TRUE},TRUE,TRUE},
  {"clr", {FALSE,FALSE,FALSE,FALSE},{FALSE,TRUE,TRUE,TRUE},FALSE,TRUE},
  {"not", {FALSE,FALSE,FALSE,FALSE},{FALSE,TRUE,TRUE,TRUE},FALSE,TRUE},
  {"inc", {FALSE,FALSE,FALSE,FALSE},{FALSE,TRUE,TRUE,TRUE},FALSE,TRUE},
  {"dec", {FALSE,FALSE,FALSE,FALSE},{FALSE,TRUE,TRUE,TRUE},FALSE,TRUE},
  {"jmp", {FALSE,FALSE,FALSE,FALSE},{FALSE,TRUE,TRUE,FALSE},FALSE,TRUE},
  {"bne", {FALSE,FALSE,FALSE,FALSE},{FALSE,TRUE,TRUE,FALSE},FALSE,TRUE},
  {"red", {FALSE,FALSE,FALSE,FALSE},{FALSE,TRUE,TRUE,TRUE},FALSE,TRUE},
  {"prn", {FALSE,FALSE,FALSE,FALSE},{TRUE,TRUE,TRUE,TRUE},FALSE,TRUE},
  {"jsr", {FALSE,FALSE,FALSE,FALSE},{FALSE,TRUE,TRUE,FALSE},FALSE,TRUE},
  {"rts", {FALSE,FALSE,FALSE,FALSE},{FALSE,FALSE,FALSE,FALSE},FALSE,FALSE},
  {"stop", {FALSE,FALSE,FALSE,FALSE},{FALSE,FALSE,FALSE,FALSE},FALSE,FALSE}
};
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

char *reserved_directives[] =
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
