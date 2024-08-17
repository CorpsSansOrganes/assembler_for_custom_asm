#include "bitmap.h"
#include "vector.h"
#include "assembler.h"
#include "utils.h"
#include "syntax_errors.h"
#include "macro_table.h"
#include "symbol_table.h"
#include <stdio.h> /* perror */
#include <string.h> 
#include <stdlib.h>


typedef enum {
  E,
  R,
  A,
} encoding_type_t;

/* 
*@brief produce the opcode of an instruction line, and adds the correct number to the instruction counter
*
*@param parameters: the operands of the instruction
*       instruction_word: the instruction word
*
*@return  vector containing 1-3 bitmap words 
*/

vector_t *InstructionLineToMachineCode(operand_t *first_operand, operand_t *second_operand, unsigned int num_of_operands, char *instruction_name, int *IC);

/* 
*@brief produce the opcode of an string directive line, i.e adds to the opcode vector an opcode line for every char and the terminating char
*
*@param opcode: the vector that contains the overall opcode 
*       string: the string to convert to opcode 
*
*@return  returns the full vector opcode containing the opcode of the string
*/

 vector_t *StringLineToMachineCode(vector_t *opcode, char *string);

/* 
*@brief produce the opcode of an data directive line, i.e adds to the opcode vector an opcode line for every int. 
*       in addition update the data counter according to number of opcodes added 
*
*@param opcode: the vector that contains the overall opcode 
*       string: the string that contains the paramaters
*       DC: address of the data counter.
*       num_of_parameters: num of data parameters
*
*@return  returns the full vector opcode containing the opcode of the data parameters
*/

vector_t *DataLineToMachineCode(vector_t *full_opcode, char *string, int *DC, int num_of_parameters);
