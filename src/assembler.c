#include "assembler.h"
#include "utils.h"
#include "syntax_errors.h"
#include "macro_table.h"
#include "symbol_table.h"
#include "generate_opcode.h"
#include "vector.h"
#include "string_utils.h"
#include "generate_output_files.h"
#include <stdio.h> /* perror */
#include <string.h> 
#include <stdlib.h>

static bool_t FirstWordEndsWithColon(char *line);
static int SymbolErrorUnite (char *symbol_name,syntax_check_config_t syntax_check_config_print, macro_table_t *macro_list, symbol_table_t *symbol_table);
static result_t FirstPass(char *file_path, macro_table_t *macro_list,
                         symbol_table_t *symbol_table, vector_t *opcode, int *IC, int *DC);
static result_t SecondPass(char *file_path, symbol_table_t *symbol_table, vector_t *opcode);
static result_t UpdateAdresses (symbol_table_t *symbol_table, int IC);
static int SplitOperands(char *line, operand_t *first_operand, operand_t *second_operand );

result_t AssembleFile(char *file_path,  macro_table_t *macro_list) {
  int IC = 0;
  int DC = 0;
  symbol_table_t *symbol_table;
  vector_t *opcode;
  symbol_table = CreateSymbolTable();
  if (NULL == symbol_table) {
    perror("Memory allocation error: couldn't allocate a symbol table\n");
    return MEM_ALLOCATION_ERROR;
  }  
  opcode = CreateVector(0,sizeof(vector_t *));
  if (NULL == opcode) {
    perror("Memory allocation error: couldn't allocate a vecotr\n");
    return MEM_ALLOCATION_ERROR;
  }  
  if (SUCCESS != FirstPass (file_path,macro_list,symbol_table, opcode, &IC, &DC)){
    DestroyMacroTable(macro_list);
    DestroySymbolTable(symbol_table);
    DestroyVector (opcode);
    return FAILURE;
  }
  DestroyMacroTable(macro_list);
  if (SUCCESS != SecondPass(file_path, symbol_table,opcode)){
    DestroySymbolTable(symbol_table);
    DestroyVector (opcode);
    return FAILURE;
  }
  if (SUCCESS != GenerateOutputFiles(opcode, symbol_table,file_path,IC,DC)){
    DestroySymbolTable(symbol_table);
    DestroyVector (opcode);
    return FAILURE;
  }
    DestroySymbolTable(symbol_table);
    DestroyVector (opcode);
    return SUCCESS;
}

static result_t FirstPass(char *file_path, macro_table_t *macro_list,
                         symbol_table_t *symbol_table, vector_t *opcode, int *IC, int *DC) {
  int line_counter = 0;
  int error_count_in_line = 0;
  int total_errors = 0;
  char *current_word = NULL; 
  char *current_line = NULL;
  char *symbol_name=NULL;
  FILE *input_file = NULL;
  char *entry_parameter = NULL;
  operand_t *first_operand = NULL;
  operand_t *second_operand = NULL;
  int num_of_operands;

  syntax_check_config_t syntax_check_config_print = CreateSyntaxCheckConfig ("file_path",
                                                 0,
                                                 TRUE);
  input_file = fopen(file_path, "r");
  if (NULL == input_file) {
    perror("Couldn't open input file");
    return ERROR_OPENING_FILE; 
  }

 
  while (NULL != fgets(current_line, MAX_LINE_SIZE, input_file)) {
      error_count_in_line = 0; 
      line_counter++;
      symbol_name = NULL;

      /* Dealing with symbol definitions */
      if (FirstWordEndsWithColon(current_line)) {
        current_word = strtok(current_line, ": ");
        symbol_name = StrDup (current_word);
        syntax_check_config_print.line_number = line_counter;
        error_count_in_line += SymbolErrorUnite(symbol_name,
                                                &syntax_check_config_print,
                                                macro_list,
                                                symbol_table);

        current_line += strlen(current_word) + 2; /*skip the colon and space*/
        current_word = strtok(NULL,blank_delimiters);
      }        
      current_word = strtok(current_line, " ");

      if (NoDefinitionForSymbol(current_line,&syntax_check_config_print)){
        error_count_in_line++;
      }
      else if (0 == strcmp(current_word,".string") ){
        current_line += strlen(current_word) + 1;
        if (IsIllegalString(current_line,&syntax_check_config_print)){ 
          error_count_in_line++; 
        }
        else if (error_count_in_line==0) {
          if (symbol_name!= NULL){
          AddSymbol (symbol_table,symbol_name,*DC,DATA);
          }
          opcode = StringLineToMachineCode(opcode, current_line); 
          *DC += strlen(current_line) - 1; /*the length of the string without the quation marks plus '/0' */
        }  
      }  
      else if (0 == strcmp(current_word,".data")) {
          current_line += strlen(current_word)+1;
          if (IsIllegalDataParameter(current_line,&syntax_check_config_print)){
                error_count_in_line++;
          }
          if (error_count_in_line == 0){
                if (symbol_name != NULL){
                  if (SUCCESS != AddSymbol (symbol_table, symbol_name,DC,DATA)){
                    return FAILURE;
                  } 
                }
                opcode = DataLineToMachineCode(opcode, current_line, &DC);
          }
      }
      else if (0 == strcmp(current_word,".extern")){
        current_line += strlen(current_word) + 1;
        if (isIllegalOrExternOrEntryParameter(current_line,symbol_table, &syntax_check_config_print)){
          error_count_in_line++;
        }
        else {
          symbol_name = strtok (NULL,blank_delimiters);
          error_count_in_line += SymbolErrorUnite(symbol_name,&syntax_check_config_print,macro_list,symbol_table);
          if (SymbolAlreadyDefinedAsExtern(symbol_name,symbol_table, &syntax_check_config_print)){
            error_count_in_line++;
          }
          if (error_count_in_line == 0){
              if (SUCCESS != AddExternalSymbol(symbol_table, symbol_name)){
                return FAILURE;
              }
              *DC += CountParameters(current_line);
          }
        }
      }
      else if (0 == strcmp(current_word,".entry")) {
        /* Do nothing: will handle in 2nd pass */
      }
      else if (FALSE == InstructionDoesntExist(current_word,&syntax_check_config_print)){
          if (0 == error_count_in_line){
            if (symbol_name != NULL){
             if (SUCCESS != AddSymbol (symbol_table, symbol_name,*IC+INITIAL_IC_VALUE, CODE)){
               return FAILURE;
             }
            }
            current_line += strlen (current_word)+1;
            num_of_operands = SplitOperands (current_line, first_operand,second_operand);
            if (WrongNumberOfOperands(current_word,num_of_operands,&syntax_check_config_print)){
              error_count_in_line++;
            }
            if (IncorrectAddressingMethod(current_word,first_operand,&syntax_check_config_print)){
              error_count_in_line++;
            }
            if (IncorrectAddressingMethod(current_word,second_operand,&syntax_check_config_print)){
              error_count_in_line++;
            }
            VectorAppend (opcode, InstructionLineToMachineCode (current_line,current_word, &IC));
         }
      }
    total_errors += error_count_in_line;
  
  }
  if (SUCCESS != UpdateAdresses (symbol_table, *IC)){
    return FAILURE;
  }
  if (0 == total_errors){
    return SUCCESS;
  }
  return FAILURE;
}

static result_t SecondPass(char *file_path, symbol_table_t *symbol_table, vector_t *opcode){
  syntax_check_config_t syntax_check_config_print = CreateSyntaxCheckConfig (file_path,
                                                 0,
                                                 TRUE);
  int line_counter = 0;
  symbol_t *symbol = NULL;
  int error_count_in_line = 0;
  int total_errors = 0;
  int vector_counter = 0;
  bitmap_t *opcode_bitmap; 
  char *current_word = NULL; 
  char *current_line = NULL;
  char *symbol_name=NULL;
  FILE *input_file = NULL;
  char *entry_parameter = NULL;
  input_file = fopen(file_path, "r");
  if (NULL == input_file) {
    perror("Couldn't open input file");
    return ERROR_OPENING_FILE; 
  }
  while (NULL != fgets(current_line, MAX_LINE_SIZE, input_file)) {
    error_count_in_line = 0; 
    line_counter++;
    symbol_name = NULL;
    vector_t *opcode_line;
    if (FirstWordEndsWithColon(current_line)) {
        current_word = strtok (current_line, blank_delimiters);
        current_line += strlen (current_word) +1;               
    }
    current_word = strtok (current_line, blank_delimiters);
    current_line += strlen (current_word) +1;      
    if (0 == strcmp (current_word, ".extern") || 0 == strcmp (current_word, ".data") || 0 == strcmp (current_word, ".string")){
        /*nothing to do, continue to the next line*/
    }
    if (0 == strcmp (current_word, ".entry")){
       current_line += strlen (current_word) +1; 
       if (isIllegalOrExternOrEntryParameter(current_line)){
          error_count_in_line++;
       }
       if (ChangeSymbolToEntry(symbol_table,current_word) != SUCCESS)
       {
          return FAILURE;
       }
    }
    else if (FALSE == InstructionDoesntExist(current_word,&syntax_check_config_print)){
        current_word = strtok (NULL,", \n\t\r");
        if (NULL != current_word){
          symbol = FindSymbol (symbol_table,current_word);
          if (NULL != symbol){
            opcode_line = GetElementVector (opcode, vector_counter);
            opcode_bitmap = GetElementVector (opcode_line,1);
            *opcode_bitmap = GetSymbolAddress (symbol);
            if (EXTERN == GetSymbolType (symbol)){
              addoccurence (symbol, line_counter);/*pseudocode, to change!*/
            }
          } 
        }
        current_word = strtok (NULL,", \n\t\r");
          if (NULL != current_word){
            symbol = FindSymbol (symbol_table,current_word);
            if (NULL != symbol){
              opcode_line = GetElementVector (opcode, vector_counter);
              opcode_bitmap = GetElementVector (opcode_line,2);
              *opcode_bitmap = GetSymbolAddress (symbol);
            } 
        }
        total_errors += error_count_in_line;
        vector_counter++;
    }
  

  }
  if (0 == total_errors){
    return SUCCESS;
  }
  return FAILURE;
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

static int SymbolErrorUnite (char *symbol_name,syntax_check_config_t *syntax_check_config_print, macro_table_t *macro_list, symbol_table_t *symbol_table){
      int internal_counter =0;
      if (SymbolNameIsIllegal (symbol_name,syntax_check_config_print)){
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

/* @brief - generating the opcode of an operand, according to its adressing method.
*           if its addressing is Direct (which means it is a symbol) 
*           nothing it will return 0, and it will be computed in the second pass
*  @param - operand: the operand
*  @return - the opcode of the operand
*/


static result_t UpdateAdresses (symbol_table_t *symbol_table, int IC){
  symbol_t *symbol = GetHeadSymbol (symbol_table);
  int address;
  if (NULL != symbol){
    address = GetSymbolAddress (symbol);
  }
  while (NULL != symbol){
    if (DATA == GetSymbolAddress(symbol)){
      if (SUCCESS != UpdateSymbolAddress (symbol,address+IC)){
        return FAILURE;
      }
    }
    symbol = GetNextSymbol (symbol);
  }
  return SUCCESS;
}

static int SplitOperands(char *line, operand_t *first_operand, operand_t *second_operand) {
    int counter = 0;
    char *current_word = strtok (line, "' /n/t/r");
    if (current_word != NULL){
      first_operand->name = StrDup (current_word);
      first_operand->addressing_method =DetectAddressingMethod(first_operand->name);
      first_operand->type = DESTINATION_OPERAND;
      counter++;
    }
    current_word = strtok (NULL, "' /n/t/r");
    if (current_word != NULL){
      second_operand->name =StrDup (current_word);
      second_operand->addressing_method =DetectAddressingMethod(second_operand->name);
      second_operand->type = DESTINATION_OPERAND;
      first_operand->type = SOURCE_OPERAND;
      counter++;
      while ('/0' != *current_word){
      if (*line == ','){
        counter++;
      }
      current_word++;
    }
    }
    return counter;
  }   



