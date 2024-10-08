/* generate_opcode.c
 *
 * This module provides utilities for computing the opcodes 
 * of various language components.
 */

#include <string.h>
#include <stdlib.h> /* atoi */
#include "assembler.h"
#include "macro_table.h"
#include "generate_opcode.h"
#include "language_definitions.h"
#include "vector.h"

#define delimiters (", /n/t/r")

static instruction_t FindInstruction(const char *instruction_name,
                                      int *instruction_number);
static int UnifyRegisterOpcode(int register_opcode_source,
                                int register_opcode_destination);
static bitmap_t OperandToOpcode(operand_t *operand);
static bitmap_t SetBitOfARE (bitmap_t bitmap, encoding_type_t ARE);
static bitmap_t SetBitAddressingMethod (bitmap_t bitmap, operand_t *operand);
static bool_t AreTwoRegitserOperands (operand_t *src_operand, operand_t *dest_operand);

#define MOVE_OPCODE_TO_PLACE(X) ((X) << 11)
#define BIT_MASK_15_BITS (0x7FFF)

result_t DataDirectiveToMachinecode(vector_t *data_table, char *params) {
  char *current_word = strtok(params, delimiters);

  while (NULL != current_word) {
    bitmap_t data_to_write = atoi(current_word) & BIT_MASK_15_BITS;
    if (MEM_ALLOCATION_ERROR == AppendVector(data_table, &data_to_write)) {
      return MEM_ALLOCATION_ERROR;
    }
    current_word = strtok (NULL, delimiters);  
  }

  return SUCCESS;
}

result_t StringDirectiveToMachinecode(vector_t *data_table, char *string) {

  /* Skip the begining quoation marks */
  string++; 
  while ('\0' != *(string + 1)) { /* Skip the end quoation marks */
    bitmap_t data_to_write = *string & BIT_MASK_15_BITS;
    if (MEM_ALLOCATION_ERROR == AppendVector(data_table, &data_to_write)) {
      return MEM_ALLOCATION_ERROR;
    }
    string++;
  }

  /* Terminating character (\0) */
  if (MEM_ALLOCATION_ERROR == AppendVector (data_table, (string + 1))) {
    return MEM_ALLOCATION_ERROR;
  }

  return SUCCESS;
}

result_t InstructionStatementToMachinecode(vector_t *code_table,
                                           const char *instruction,
                                           operand_t *source_operand,
                                           operand_t *dest_operand) {
  bitmap_t instruction_opcode = 0;
  bitmap_t src_operand_opcode = 0;
  bitmap_t dest_operand_opcode = 0;
  int instruction_number = 0;

  /* Set up the first word (instruction word) */
  FindInstruction(instruction, &instruction_number);
  instruction_opcode += instruction_number;

  /* Put the instruction number in place */
  instruction_opcode = MOVE_OPCODE_TO_PLACE(instruction_opcode);
  instruction_opcode = SetBitOfARE(instruction_opcode, A);
  instruction_opcode = SetBitAddressingMethod(instruction_opcode, source_operand); 
  instruction_opcode = SetBitAddressingMethod(instruction_opcode, dest_operand);
  instruction_opcode &= BIT_MASK_15_BITS;

  if (MEM_ALLOCATION_ERROR == AppendVector(code_table, &instruction_opcode)) {
    return MEM_ALLOCATION_ERROR;
  }

  /* Handling coding when we have two operands */
  if (NULL != source_operand && NULL != dest_operand) {
    src_operand_opcode = OperandToOpcode(source_operand);
    dest_operand_opcode = OperandToOpcode(dest_operand);

    /* If we have two registers as operands, they are encoded in a single block */
    if (AreTwoRegitserOperands(source_operand, dest_operand)) {
      bitmap_t registers_block = UnifyRegisterOpcode(src_operand_opcode, dest_operand_opcode);
      registers_block &= BIT_MASK_15_BITS;

      if (MEM_ALLOCATION_ERROR == AppendVector(code_table, &registers_block)) {
        return MEM_ALLOCATION_ERROR;
      }
    }
    /* At least one isn't a register, we encode two blocks. */
    else {
      src_operand_opcode &= BIT_MASK_15_BITS;
      if (MEM_ALLOCATION_ERROR == AppendVector (code_table, &src_operand_opcode)) {
        return MEM_ALLOCATION_ERROR;
      }
      dest_operand_opcode &= BIT_MASK_15_BITS;
      if (MEM_ALLOCATION_ERROR == AppendVector (code_table, &dest_operand_opcode)) {
        return MEM_ALLOCATION_ERROR;
      }
    }
  }

  /* Handling coding when we have only one operand (always target) */
  else if (NULL != dest_operand) { 
    dest_operand_opcode = OperandToOpcode(dest_operand);
    if (MEM_ALLOCATION_ERROR == AppendVector (code_table, &dest_operand_opcode)) {
        return MEM_ALLOCATION_ERROR;
    }
  }

  return SUCCESS;
}  

/* 
*@brief set the bit of the correct addressing method int the opcode of the instruction word
*
*@param bitmap - the bitmap of the onstruction 
*       operand - the operand of the instruction
*
*@return The bitmap with the correct bit turned on. If operand is NULL, nothing happens.
*/

static bitmap_t SetBitAddressingMethod (bitmap_t bitmap, operand_t *operand){
  if (NULL == operand) {
    return bitmap;
  }

  if (SOURCE_OPERAND == operand->type) {
    bitmap = SetBitOn(bitmap, STARTING_BIT_SRC_OPERAND + operand->addressing_method);
  }
  else {
    bitmap = SetBitOn (bitmap, STARTING_BIT_DEST_OPERAND + operand->addressing_method);
  }

  return bitmap;
}

/* 
*@brief set the bit of the correct encoding type, where A=2, R=1, E=0
*
*@param bitmap: the bitmap of the onstruction 
*       ARE: the encoding type.
*
*@return the bitmap with the correct bit turned on
*/

static bitmap_t SetBitOfARE (bitmap_t bitmap, encoding_type_t ARE){
    bitmap = SetBitOn(bitmap, ARE);
    return bitmap;
} 



static instruction_t FindInstruction(const char *instruction_name,
                                     int *instruction_number) {
  int i = 0;

  while (0 != strcmp(reserved_instructions[i].name, instruction_name)) {
    ++i;
  }

  *instruction_number = i;

  return reserved_instructions[i];
}

/* @brief Generating one opcode out of two opcodes that belongs to register operands. 
*         doesnt matter if its direct or indirect
*
*  @param register_opcode_source - The opcode of the register source operand
*         register_opcode_destination - The opcode of the register destination operand
*
*  @return The opcode of the operand.
*/

static int UnifyRegisterOpcode (int register_opcode_source,
                                int register_opcode_destination) {
  /*opcode of source is in 3-5, opcode of destination 6-8.
  minus 4 represents the A value which is set on both*/
  return (register_opcode_source+register_opcode_destination - 4);
}             

static bitmap_t OperandToOpcode(operand_t *operand){
  bitmap_t opcode = 0;

  if (IMMEDIATE == operand->addressing_method) {
    /* Skip # */
    opcode = atoi(operand->name + 1);
    opcode = opcode << 3; /* make space for ARE */
    opcode = SetBitOfARE(opcode,A); /*A=1, R=0, E=0*/
    return opcode;
  }

  else if (DIRECT == operand->addressing_method) {
    /* Depend by the symbol table, will be handled by second pass */
    return 0;
  }

  /* In/direct register addresing */
  else
  {
    if (operand->addressing_method == DIRECT_REGISTER) {
      /* Skip r to get its index. e.g. "r3" -> "3" */
      opcode = atoi(operand->name + 1);
    } 
    else {
      /* Skip *r to get its index. e.g. "*r3" -> "3" */
      opcode = atoi(operand->name + 2);
    }

    /* Source register number is stored in bits 6 - 8. */
    if (SOURCE_OPERAND == operand->type) {
      opcode = opcode << 6;
    }

    /* Target register number is stored in bits 3 - 5. */
    else {
      opcode = opcode << 3;
    }
    opcode = SetBitOfARE(opcode,A); /*A=1, R=0, E=0*/
  }

  return opcode;
}

static bool_t AreTwoRegitserOperands(operand_t *src_operand, operand_t *dest_operand) {
   if ((src_operand->addressing_method == DIRECT_REGISTER || src_operand->addressing_method == INDIRECT_REGISTER) &&
       (dest_operand->addressing_method == DIRECT_REGISTER || dest_operand->addressing_method == INDIRECT_REGISTER)) {
    return TRUE;
  }

  return FALSE;
}
