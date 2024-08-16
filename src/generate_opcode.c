#include "assembler.h"
#include "macro_table.h"
#include "generate_opcode.h"
#include "vector.h"
#include <string.h> 

static instruction_t FindInstruction (char *instruction_name, int *instruction_number);
static int UnifyRegisterOpcode (int register_opcode_source, int register_opcode_destination);
static int OperandToOpcode(operand_t *operand);

vector_t *DataLineToMachineCode(vector_t *full_opcode, char *string, int *DC, int num_of_parameters){
    int i;
    bitmap_t parameter = atoi (strtok (string, ", /n/t/r"));
    VectorAppend (full_opcode,parameter);
    for (i=1; i<num_of_parameters;i++){
        parameter = atoi (strtok (NULL, ", /n/t/r"));
        VectorAppend (full_opcode,parameter);
    }
    DC += num_of_parameters;
    return full_opcode;
}

vector_t *StringLineToMachineCode(vector_t *full_opcode, char *string){
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


vector_t *InstructionLineToMachineCode(operand_t *first_operand, operand_t *second_operand, unsigned int num_of_operands, char *instruction_name, int *IC){
vector_t *line_machine_code = CreateVector (0,sizeof(bitmap_t));
bitmap_t instruction_opcode = 0;
bitmap_t operand_opcode = 0;
int L; /*number of memory words needed*/
int instruction_number;
instruction_t current_instruction;
current_instruction = FindInstruction (instruction_name, &instruction_number); 
instruction_opcode += instruction_number;
instruction_opcode = instruction_opcode <<11;
instruction_opcode = SetBitOfARE (instruction_opcode,A);
if (0 == num_of_operands){
      VectorAppend (line_machine_code, instruction_opcode);
      L=1;
}
if (1 == num_of_operands){
    L=2;
    instruction_opcode =SetBitAddressingMethod (instruction_opcode, first_operand); 
    operand_opcode = OperandToOpcode (first_operand);
    VectorAppend (line_machine_code, instruction_opcode);
    VectorAppend (line_machine_code, operand_opcode);
}
  if (2 == num_of_operands){
    instruction_opcode = SetBitAddressingMethod (instruction_opcode, first_operand); 
    instruction_opcode = SetBitAddressingMethod (instruction_opcode, second_operand);
    VectorAppend (line_machine_code, instruction_opcode);
    if ((first_operand->addressing_method == DIRECT_REGISTER || first_operand->addressing_method == INDIRECT_REGISTER) &&
        (second_operand->addressing_method == DIRECT_REGISTER || second_operand->addressing_method == INDIRECT_REGISTER)){
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
IC += L; 
return line_machine_code;
}  

bitmap_t SetBitAddressingMethod (bitmap_t bitmap, operand_t *operand){
   if (operand->type == SOURCE_OPERAND){
   bitmap = SetBitOn (bitmap, 3+operand->addressing_method);
   }
   else {
    bitmap = SetBitOn (bitmap, 7+operand->addressing_method);
   }
   return bitmap;
}

bitmap_t SetBitOfARE (bitmap_t bitmap, encoding_type_t ARE){
    bitmap = SetBitOn (bitmap, ARE);
} 



static instruction_t FindInstruction (char *instruction_name, int *instruction_number){
 int i =0;
  while (0 != strcmp(reserved_instructions[i].name, instruction_name)) {
    ++i;
  }
  instruction_number = i;
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
      
static int OperandToOpcode(operand_t *operand){/* TODO add number too big error */
  char *extract_operand;
  int opcode =0;
  if (operand->addressing_method == IMMEDIATE)
  {
    if (*(operand->name+1) == '-' || *(operand->name+1) == '+'){
      opcode = atoi (operand->name+2);
    }
    else{
      opcode = atoi (operand->name+1);
    }
    opcode = opcode << 3;/*make space for ARE*/
    opcode += 4; /*add A=1, R=0, E=0*/
    return opcode;
  }
  if (operand->addressing_method == DIRECT){
    /*depend by the symbol table, to handle in second pass*/
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
    opcode += 4; /*add A=1, R=0, E=0( add macro A)*/
  }
return opcode;
}
