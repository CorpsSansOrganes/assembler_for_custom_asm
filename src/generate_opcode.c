#include "assembler.h"
#include "utils.h"
#include "utils.h"
#include "syntax_errors.h"
#include "macro_table.h"
#include "symbol_table.h"
#include "generate_opcode.h"
#include "vector.h"
#include <stdio.h> /* perror */
#include <string.h> 
#include <stdlib.h>

static instruction_t FindInstruction (char *instruction_name);
static int CountParameters(char *line);

vector_t DataLineToMachineCode(vector_t full_opcode, char *string, int num_of_parameters){
    int i;
    bitmap_t parameter = atoi (strtok (string, ", /n/t/r"));
    VectorAppend (full_opcode,parameter);
    for (i=1; i<num_of_parameters;i++){
        parameter = atoi (strtok (NULL, ", /n/t/r"));
        VectorAppend (full_opcode,parameter);
    }
    return full_opcode;
}

vector_t StringLineToMachineCode(vector_t full_opcode, char *string){
    bitmap_t char_opcode;
    string++; /*skip the begining quation marks*/
    while ('\0' != *string+1)/*skip the end quation marks*/
    {
        char_opcode = *string;
        string++;
        VectorAppend (full_opcode, char_opcode); 
    }
    char_opcode = 0;
    VectorAppend (full_opcode, char_opcode); 
    return full_opcode;
}


vector_t InstructionLineToMachineCode(char *operands, char *instruction_name, int *IC){
vector_t line_machine_code = VectorCreate (0,sizeof(bitmap_t));
bitmap_t instruction_opcode = 0;
bitmap_t operand_opcode = 0;
int L; /*number of memory words needed*/
unsigned int num_of_operands;
char *current_operand = NULL;
operand_t first_operand;
operand_t second_operand;
int instruction_number;
instruction_t current_instruction;
current_instruction = FindInstruction (instruction_name, &instruction_number); 
instruction_opcode += instruction_number;
instruction_opcode = instruction_opcode <<11;
instruction_opcode = SetBitOfARE (instruction_opcode,A);
num_of_operands = CountParameters (operands);
if (FALSE == WrongNumberOfOperands (current_instruction.name,num_of_operands,syntax_check_config_print)){
  if (0 == num_of_operands){
      VectorAppend (line_machine_code, instruction_opcode);
      L=1;
  }
  if (1 == num_of_operands){
    L=2;
    current_operand = strtok(operands, blank_delimiters);
    first_operand.name = current_operand;
    first_operand.addressing_method = DetectAddressingMethod(current_operand);
    first_operand.type = DESTINATION_OPERAND;
    operand_opcode = OperandToOpcode (first_operand);
    instruction_opcode =SetBitAddressingMethod (instruction_opcode, first_operand); 
    VectorAppend (line_machine_code, instruction_opcode);
    VectorAppend (line_machine_code, operand_opcode);

    /*insert opcode*/
}
  if (2 == num_of_operands){
    current_operand = strtok(operands, ", \t\n\r");/*TODO check error of the case 'operand1 ,, operand2'*/
    first_operand.name = current_operand;
    first_operand.addressing_method = DetectAddressingMethod(current_operand);
    first_operand.type = SOURCE_OPERAND;
    instruction_opcode = SetBitAddressingMethod (instruction_opcode, first_operand); 
    current_operand = strtok(NULL, ", \t\n\r");
    second_operand.name = current_operand;
    second_operand.addressing_method = DetectAddressingMethod(current_operand);
    second_operand.type = DESTINATION_OPERAND;
    instruction_opcode = SetBitAddressingMethod (instruction_opcode, second_operand);
    VectorAppend (line_machine_code, instruction_opcode);
      if ((first_operand.addressing_method == DIRECT_REGISTER || first_operand.addressing_method == INDIRECT_REGISTER) &&
        (second_operand.addressing_method == DIRECT_REGISTER || second_operand.addressing_method == INDIRECT_REGISTER)){
        operand_opcode = UnifyRegisterOpcode (OperandToOpcode (first_operand),OperandToOpcode (second_operand));
        VectorAppend (line_machine_code, operand_opcode);
        L=2;
        }
      else {
        operand_opcode = OperandToOpcode (first_operand);
        VectorAppend (line_machine_code, operand_opcode);
        operand_opcode = OperandToOpcode (second_operand);
        VectorAppend (line_machine_code, operand_opcode);
        L=3;
    }
   }
   
} 
IC += L; 
return line_machine_code;
}  

bitmap_t SetBitAddressingMethod (bitmap_t bitmap, operand_t operand){
   if (operand.type == SOURCE_OPERAND){
   bitmap = SetBitOn (bitmap, 3+operand.addressing_method);
   }
   else {
    bitmap = SetBitOn (bitmap, 7+operand.addressing_method);
   }
   return bitmap;
}

bitmap_t SetBitOfARE (bitmap_t bitmap, encoding_type_t ARE){
    bitmap = SetBitOn (bitmap, ARE);
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

static instruction_t FindInstruction (char *instruction_name, int *instruction_number){
 int i =0;
  while (0 != strcmp(reserved_instructions[i].name, instruction_name)) {
    ++i;
  }
  instruction_name = i;
  return reserved_instructions[i];

}