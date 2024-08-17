#ifndef __SH_ED_LANGUAGE_DEFINITIONS__
#define __SH_ED_LANGUAGE_DEFINITIONS__

#include "utils.h"
#include "bitmap.h"

#define NUM_OF_INSTRUCTIONS 16
#define NUM_OF_DIRECTIVES 4
#define NUM_OF_REGISTERS 8

#define SYMBOL_CHARACTER_LIMIT 31

/*
 * The value of an immediate operand are stored in bits 3 - 15 of the word.
 * Since 2's complement is employed, the maximum value which can be represented
 * is:
 *   2^11-1 = 0111 1111 1111 = 2047.
 *
 * and the minimal value is:
 *   -2^11 = 1000 0000 0000 = -2048.
 */

#define MAX_IMMEDIATE_OPERAND_NUM (2047) 
#define MIN_IMMEDIATE_OPERAND_NUM (-2048) 

#define MAX_DATA_PARAMETER 16384 /*2^14: 15 -1 for negative number*/


/* Line length in the source code cannot be more than 80 characters + \n */
#define MAX_LINE_LENGTH 81

/*
 * @brief Definitions for reserved words and other building blocks
 * of the supported assembly language.
 */

typedef enum {
  SOURCE_OPERAND,
  DESTINATION_OPERAND
} operand_type_t;

typedef enum {
  IMMEDIATE,         /* #3     */
  DIRECT,            /* SYMBOL */
  INDIRECT_REGISTER, /* *r2    */
  DIRECT_REGISTER,   /* r3     */
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


typedef struct {
  const char *name;
  addressing_method_t addressing_method;
  operand_type_t type;
} operand_t;

extern instruction_t reserved_instructions[NUM_OF_INSTRUCTIONS];

extern char *reserved_directives[NUM_OF_DIRECTIVES];

extern char *register_names[NUM_OF_REGISTERS];

#endif /* __SH_ED_LANGUAGE_DEFINITIONS__ */
