#ifndef __LANGUAGE_DEFINITIONS__
#define __LANGUAGE_DEFINITIONS__

#include "utils.h"
#include "bitmap.h"

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

} addressing_method_t;

/*
* @brief Struct representing an instruction & its properties.
*
*   Each instruction is characterised by:
*     1. A name (e.g. "mov")
*     2. Which operands does it take (source, destination, none or both).
*     3. Legal addressing method for each operand.
*
* @param name - The name of the instruction.
*        addressing_info - Bitmap containing properties about legal addressing 
*                          methods. The bitmap is structed as follows:
*        
*        OPERAND BITS
*        Bits 0 - 1 indicates if the instruction takes:
*         0 - a source operand.
*         1 - a destination operand.
*
*        SOURCE OPRAND ADDRESSING METHODS
*        Bits 2 - 5 indicates if the source operand uses:
*         2 - Immediate addressing (e.g. mov #-1, r2)
*         3 - Direct addressing (e.g. dec x, when x is a label that has been defined).
*         4 - Indirect register addressing (e.g. inc *r1)
*         5 - Direct register addressing (e.g. clr r1).
*
*        DESTINATION OPERAND ADDRESSING METHODS
*        Bits 6 - 9 indicates if the destination operand uses:
*         6 - Immediate addressing 
*         7 - Direct addressing 
*         8 - Indirect register addressing 
*         9 - Direct register addressing 
*        
*
*         If bit 0 is off, then the values of bits 2-5 are undefined.
*         Similarly, if bit 1 is off, the values of bits 6-9 are undefined.
*/

typedef struct {
  const char *name;
  bitmap_t addressing_info;
 } instruction_t;

instruction_t reserved_instructions [NUM_OF_INSTRUCTIONS] = {
  {"mov", 959},  /* 0b1110111111 */
  {"cmp", 1023}, /* 0b1111111111 */ 
  {"add", 959},  /* 0b1110111111 */
  {"sub", 959},  /* 0b1110111111 */
  {"lea", 907},  /* 0b1110001011 */
  {"clr", 898},  /* 0b1110000010 */
  {"not", 898},  /* 0b1110000010 */
  {"inc", 898},  /* 0b1110000010 */
  {"dec", 898},  /* 0b1110000010 */
  {"jmp", 386},  /* 0b0110000010 */
  {"bne", 386},  /* 0b0110000010 */
  {"red", 898},  /* 0b1110000010 */
  {"prn", 962},  /* 0b1111000010 */
  {"jsr", 386},  /* 0b0110000010 */
  {"rts", 0},    /* 0b0000000000 */
  {"stop", 0}   /* 0b0000000000 */
};

char *reserved_directives[] =
  {
    ".data",
    ".entry",
    ".string",
    ".extern"
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
    "r7"
  };

#endif /* __LANGUAGE_DEFINITIONS__ */
