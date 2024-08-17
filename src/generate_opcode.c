#include <string.h>
#include <stdlib.h> /* atoi */
#include "assembler.h"
#include "macro_table.h"
#include "generate_opcode.h"
#include "vector.h"
#define delimiters ", /n/t/r" 

static instruction_t FindInstruction (char *instruction_name, int *instruction_number);
static int UnifyRegisterOpcode (int register_opcode_source, int register_opcode_destination);
static int OperandToOpcode(operand_t *operand);
static bitmap_t SetBitOfARE (bitmap_t bitmap, encoding_type_t ARE);
static bitmap_t SetBitAddressingMethod (bitmap_t bitmap, operand_t *operand);


result_t DataDirectiveToMachinecode(vector_t *data_table, char *params){
    char *current_word = strtok (params, delimiters);
    bitmap_t parameter = atoi(current_word);/* parameter into number*/
    VectorAppend(data_table,parameter);
    while (NULL != current_word) {
      current_word =strtok (NULL, ", /n/t/r");  
      parameter = atoi (current_word);
      VectorAppend (data_table,parameter);
    }
    return data_table;
}

result_t StringDirectiveToMachinecode(vector_t *data_table, char *string){
    bitmap_t char_opcode;
    string++; /*skip the begining quoation marks*/
    while ('\0' != *string+1)/*skip the end quoation marks*/
    {
        char_opcode = *string;
        string++;
        VectorAppend (data_table, char_opcode); 
    }
    char_opcode = 0;
    VectorAppend (data_table, char_opcode); 
    return data_table;
}


result_t InstructionStatementToMachinecode(vector_t *code_table,
                                           const char *instruction,
                                           operand_t *source_operand,
                                           operand_t *dest_operand){
vector_t *line_machine_code = CreateVector (0,sizeof(bitmap_t));
bitmap_t instruction_opcode = 0;
bitmap_t src_operand_opcode = 0;
bitmap_t dest_operand_opcode = 0;
int instruction_number;
instruction_t current_instruction = FindInstruction (instruction, &instruction_number);

instruction_opcode += instruction_number;
instruction_opcode = instruction_opcode <<11;/*put the instruction number in place*/
instruction_opcode = SetBitOfARE (instruction_opcode,A);
instruction_opcode = SetBitAddressingMethod (instruction_opcode, source_operand); 
instruction_opcode = SetBitAddressingMethod (instruction_opcode, dest_operand);
if (AppendVector (line_machine_code, instruction_opcode)){
  return FAILURE;
}


if (source_operand != NULL && dest_operand != NULL){
    src_operand_opcode = OperandToOpcode (source_operand);
    dest_operand_opcode = OperandToOpcode (dest_operand);
  if (AreTwoRegitserOperands(source_operand, dest_operand)){
    if (AppendVector (code_table, UnifyRegisterOpcode (src_operand_opcode,dest_operand_opcode))){
      return FAILURE;
    }
  }
  else {
    if (AppendVector (code_table,src_operand_opcode)){
      return FAILURE;
    }
    if (AppendVector (code_table,dest_operand_opcode)){
      return FAILURE;
    }
  }
}
else if (dest_operand != NULL){
  dest_operand_opcode = OperandToOpcode (dest_operand);
  if (AppendVector (line_machine_code, dest_operand_opcode)){
      return FAILURE;
  }
}
return SUCCESS;
}  

/* 
*@brief set the bit of the correct addressing method int the opcode of the instruction word
*
*@param bitmap: the bitmap of the onstruction 
*       operand: the operand of the instruction
*
*@return the bitmap with the correct bit turned on
*/

static bitmap_t SetBitAddressingMethod (bitmap_t bitmap, operand_t *operand){
   if (operand != NULL){
      if (operand->type == SOURCE_OPERAND){
      bitmap = SetBitOn (bitmap, 3+operand->addressing_method);
      }
      else {
        bitmap = SetBitOn (bitmap, 7+operand->addressing_method);
      }
      return bitmap;
   }
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
    bitmap = SetBitOn (bitmap, ARE);
    return bitmap;
} 



static instruction_t FindInstruction (char *instruction_name, int *instruction_number){
 int i =0;
  while (0 != strcmp(reserved_instructions[i].name, instruction_name)) {
    ++i;
  }
  *instruction_number = i;
  return reserved_instructions[i];

}
/* @brief - generating one opcode out of two opcodes that belongs to register operands. 
*           doesnt matter if its direct or indirect
*  @param - register_opcode_source: the opcode of the register source operand
*           register_opcode_destination: the opcode of the register destination operand
*  @return - the opcode of the operand
*/
static int UnifyRegisterOpcode (int register_opcode_source, int register_opcode_destination){
  /*opcode of source is in 3-5, opcode of destination 6-8.
  minus 4 represents the A value which is set on both*/
  return (register_opcode_source+register_opcode_destination - 4);
}             
      
static int OperandToOpcode(operand_t *operand){
  int opcode =0;
  if (operand->addressing_method == IMMEDIATE)
  {
    
    opcode = atoi (operand->name+1);
    opcode = opcode << 3;/*make space for ARE*/
    opcode = SetBitOfARE(opcode,A); /*A=1, R=0, E=0*/
    return opcode;
  }
  if (operand->addressing_method == DIRECT){
    /*depend by the symbol table, to handle in second pass and not in here*/
  }
  if (operand->addressing_method == DIRECT_REGISTER || operand->addressing_method == INDIRECT_REGISTER)
  {
    if (operand->addressing_method == DIRECT_REGISTER){
      opcode = atoi (operand->name+1);
    } 
    else {
      opcode = atoi (operand->name+2);
    }
    if (operand->type == SOURCE_OPERAND){
      opcode = opcode << 6;
    }
    else {
      opcode = opcode << 3;
    }
    opcode = SetBitOfARE(opcode,A); /*A=1, R=0, E=0*/
  }
return opcode;
}

static int CountParameters(char *line) {
    int counter = 1;
    while ('\0' != *line){
      line++; 
      if (*line == ','){
        counter++;
      }
    }
    return counter;
}
static bool_t AreTwoRegitserOperands (operand_t *src_operand, operand_t *dest_operand){
   if ((src_operand->addressing_method == DIRECT_REGISTER || src_operand->addressing_method == INDIRECT_REGISTER) &&
        (dest_operand->addressing_method == DIRECT_REGISTER || dest_operand->addressing_method == INDIRECT_REGISTER)){
          return TRUE;
    return FALSE;
  }
}

