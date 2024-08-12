#include "assembler.h"
#include "utils.h"
#include "syntax_errors.h"
#include "macro_table.h"
#include "symbol_table.h"
#include "generate_opcode.h"
#include "vector.h"
#include <stdio.h> /* perror */
#include <string.h> 
#include <stdlib.h>

static bool_t FirstWordEndsWithColon(char *line);
static int SymbolErrorUnite (char *symbol_name,syntax_check_config_t syntax_check_config_print, macro_table_t *macro_list, symbol_table_t *symbol_table);
static result_t FirstPass(char *file_path, macro_table_t *macro_list);
static addressing_method_t DetectAddressingMethod(const char *operand);
static int UnifyRegisterOpcode (int register_opcode_source, int register_opcode_destination);

result_t AssembleFile(char *file_path) {
  return 0; // TODO
}

static result_t FirstPass(char *file_path, macro_table_t *macro_list) {
  vector_t opcode = VectorCreate(0,size_of(vector_t *));
  int line_counter = 0;
  int error_count_in_line = 0;
  int total_errors = 0;
  int IC = INITIAL_IC_VALUE;
  int DC = 0;
  char *current_word = NULL; 
  char *current_line = NULL;
  char *symbol_name=NULL;
  FILE *input_file = NULL;
  char *entry_parameter = NULL;

  

  syntax_check_config_t *syntax_check_config_print = CreateSyntaxCheckConfig ("file_path",
                                                 0,
                                                 TRUE);
  syntax_check_config_t *syntax_check_config__no_print = CreateSyntaxCheckConfig ("default",
                                                  0,
                                                  FALSE);

  symbol_table_t *symbol_table = CreateSymbolTable();
  if (NULL == symbol_table) {
    perror("Memory allocation error: couldn't allocate a macro table\n");
    DestroyMacroTable(macro_list);
    return MEM_ALLOCATION_ERROR;
  }

  input_file = fopen(file_path, "r");
  if (NULL == input_file) {
    perror("Couldn't open input file");
    DestroyMacroTable(macro_list);
    DestroySymbolTable(symbol_table);
    return ERROR_OPENING_FILE; 
  }

 
  while (NULL != fgets(current_line, MAX_LINE_SIZE, input_file)) {
      error_count_in_line = 0; 
      line_counter++;
      symbol_name = NULL;

      /* Dealing with symbol definitions */
      if (FirstWordEndsWithColon(current_line)) {
        current_word = strtok(current_line, ": ");
        symbol_name = current_word;
        syntax_check_config_print -> line_number = line_counter;
        error_count_in_line += SymbolErrorUnite(symbol_name,
                                                syntax_check_config_print,
                                                macro_list,
                                                symbol_table);

        current_line += strlen(current_word) + 2; /*skip the colon and space*/
        current_word = strtok(NULL,blank_delimiters);
      }        
      current_word = strtok(current_line, " ");
      if (NULL == current_word){
        /*TODO print error accordingly */
      }
      else if (0 == strcmp(current_word,".string") ){
        current_line += strlen(current_word) + 1;
        if (IsIllegalString(current_line,syntax_check_config_print)){ 
          error_count_in_line++; 
        }
        else if (error_count_in_line==0) {
          if (symbol_name!= NULL){
          AddSymbol (symbol_table,symbol_name,DC);
          }
          opcode = StringLineToMachineCode(opcode, current_line); 
          DC += strlen(current_line) - 1; /*the length of the string without the quation marks plus '/0' */
        }  
      }  
      else if (0 == strcmp(current_word,".data")) {
          current_line += strlen(current_word)+1;
          if (IsIllegalDataParameter(current_line,syntax_check_config_print)){
                error_count_in_line++;
          }
          if (error_count_in_line == 0){
                if (symbol_name != NULL){
                  if (SUCCESS != AddSymbol (symbol_table, symbol_name,IC)){
                    return FAILURE;
                  } 
                }
                opcode = DataLineToMachineCode(opcode, current_line, &DC);
          }
      }
      else if (0 == strcmp(current_word,".extern")){
        current_line += strlen(current_word) + 1;
        symbol_name = strtok (NULL,blank_delimiters);/*TODO check error*/
        error_count_in_line += SymbolErrorUnite(symbol_name,syntax_check_config_print,macro_list,symbol_table);
        if (SymbolAlreadyDefinedAsExtern(symbol_name,symbol_table, syntax_check_config_print)){
          error_count_in_line++;
        } 
        if (DetectExtraCharacters(current_line+strlen (symbol_name),syntax_check_config_print)){
            error_count_in_line++;
        }
        if (error_count_in_line == 0){
            if (SUCCESS != AddExternalSymbol(symbol_table, symbol_name)){
              return FAILURE;
            }
            DC += CountParameters(current_line);
        }
      }
      else if (0 == strcmp(current_word,".entry")) {
        /* Do nothing: will handle in 2nd pass */
      }
      else if (FALSE == InstructionDoesntExist(current_word,syntax_check_config_print)){
          if (0 == error_count_in_line){
            if (symbol_name != NULL){
             if (SUCCESS != AddSymbol (symbol_table, symbol_name,IC)){
               return FAILURE;
             }
            }
            current_line += strlen (current_word)+1;
            VectorAppend (opcode, InstructionLineToMachineCode (current_line,current_word, &IC));
         }
      }
    total_errors += error_count_in_line;
    
  }
}

static result_t SecondPass(char *file_path, symbol_table_t *symbol_table){
  int line_counter = 0;
  int error_count_in_line = 0;
  int total_errors = 0;
  int IC = INITIAL_IC_VALUE;
  int DC = 0;
  char *current_word = NULL; 
  char *current_line = NULL;
  char *symbol_name=NULL;
  FILE *input_file = NULL;
  char *entry_parameter = NULL;

  if (FirstWordEndsWithColon(current_line)) {
      /*continue to the next word*/
      }
  if (/*word == extern/data/string*/){
      /*move to next line*/
  }
  if (/*word == .entry*/){
    /*change to entry the symbol*/
  }
  if (/*word == instruction*/){
    /*look for symbol and replace*/
  }


}

/* @brief - if the first word in a string ends with ':'
*  @param - line: the line
   @return - true if ends with colon, otherwise, false.
*/
static bool_t FirstWordEndsWithColon(char *line) {
    while (*(line+1) != '\t' || *(line+1) != ' ' || *(line+1) != '\0'){
       line++;
    }
    if (*line == ':'){
	      return TRUE;
    }
    return FALSE;
}

/* @brief - count parameters which seperated by ',';
*  @param - line: the line
*  @return - the number of parameters detected
*/

static int SymbolErrorUnite (char *symbol_name,syntax_check_config_t syntax_check_config_print, macro_table_t *macro_list, symbol_table_t *symbol_table){
      int internal_counter =0;
      if (SymbolPrefixIllegal (symbol_name,syntax_check_config_print)){
          internal_counter++;
      }
      if (SymbolIsIllegal (symbol_name,syntax_check_config_print)){
          internal_counter++;
      }
      if (SymbolExceedCharacterLimit (symbol_name,syntax_check_config_print)){
          internal_counter++;
      }
      if (SymbolExceedCharacterLimit (symbol_name,syntax_check_config_print)){
          internal_counter++;
      }
      if (SymbolUsedAsAMacro (symbol_name, macro_list, syntax_check_config_print)){
          internal_counter++;
      }
      if (SymbolDefinedMoreThanOnce(symbol_name,symbol_table, syntax_check_config_print)){
          internal_counter++;
      }
      return internal_counter;
}

/*copied from syntax_errors.c maybe better to put it in utils?*/
static addressing_method_t DetectAddressingMethod(const char *operand) {
  char *ptr = (char *)operand;

  if ('#' == *ptr) {
    int i = 0;
    ptr++;
    if ('-' == *ptr | '+' ==  *ptr) {
      ptr++;
    }
    for (i = 0; i < strlen(ptr); i++) {
      if (!isdigit(*(ptr+i))) {
        return INVALID;
      }
    }
    if (0 == i) { /* strlen(ptr) == 0 */ 
      return INVALID;
    }
    return IMMEDIATE;                
  }
} 
static int OperandToOpcode(operand_t operand){/*operand_t *? TODO add number too big error */
  char *extract_operand;
  int opcode =0;
  if (operand.addressing_method == IMMEDIATE)
  {
    if (*(operand.name+1) == '-' || *(operand.name+1) == '+'){
      opcode = atoi (operand.name+2);
    }
    else{
      opcode = atoi (operand.name+1);
    }
    opcode = opcode << 3;/*make space for ARE*/
    opcode += 4; /*add A=1, R=0, E=0*/
    return opcode;
  }
  if (operand.addressing_method == DIRECT){

    /*depend by the symbol table, to handle in second pass*/
  }
  if (operand.addressing_method == DIRECT_REGISTER || operand.addressing_method == INDIRECT_REGISTER)
  {
    if (operand.addressing_method == DIRECT_REGISTER){
      opcode = atoi (operand.name+1);
    } 
    else {
      opcode = atoi (operand.name+2);
    }
    if (operand.type == SOURCE_OPERAND){
      opcode = opcode << 6;
    }
    else {
      opcode = opcode << 3;
    }
    opcode += 4; /*add A=1, R=0, E=0( add macro A)*/
  }
return opcode;
}
static int UnifyRegisterOpcode (int register_opcode_source, int register_opcode_destination){
  return (register_opcode_source+register_opcode_destination - 4);
}

                /*instruction handling:
                * check errors
                * extract the operands               
                * get addressing method of operands
                * check errors
                * compute L
                * compute opcode of operands
                */
              
      

static void HandleSymbol (char *symbol, int line_number){
}
