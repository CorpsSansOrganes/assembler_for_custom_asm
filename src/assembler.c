#include "assembler.h"
#include "utils.h"
#include "syntax_errors.h"
#include "macro_table.h"
#include "symbol_table.h"
#include <string.h>
#define  blank_delimiters " \t\n\r"

result_t AssembleFile(char *file_path) {
  return 0; // TODO
}
/*notes to self:
label - always in the beginning of a a line.
if it is a data or string line - the label gets the current DC. 
if it is instruction line - gets the current IC.
youre not adding entry symbol, in the second pass you change exist symbol to entry
*/
static result_t FirstPass(char *file_path, macro_table_t *macro_list) {
  int line_counter = 0;
  int tmp_error_count = 0;
  int total_errors = 0;
  int IC = INITIAL_IC_VALUE;
  int DC = 0;
  int saved_space = 0;
  char *current_word = NULL; 
  char *current_line = NULL;
  char *symbol_name=NULL;
  char input_file = NULL;
  char *entry_parameter = NULL;

  syntax_check_info_t syntax_check_info_print = {.line_number = 0, .verbose=TRUE, .file_name = "file_path"};
  syntax_check_info_t syntax_check_info__no_print = {.line_number = -1, .verbose=FALSE, .file_name = "default"};
  symbol_table_t *symbol_table = CreateSymbolTable();/*check error*/
  if (NULL == symbol_table) {
    perror("Memory allocation error: couldn't allocate a macro table\n");
    DestroyMacroTable(macro_list);
    return NULL;
  }
  input_file = fopen(file_path, "r");
  if (NULL == input_file) {
    perror("Couldn't open input file");
    DestroyMacroTable(macro_list);
    DestroySymbolTable(symbol_table);
    return NULL;
  }
  /*
  1. read line*/
  while (NULL != fgets(current_line, MAX_LINE_SIZE, input_file)) {
    tmp_error_count =0;
    line_counter++;
  /*2. is first word finishes with a colon?*/
    if (FirstWordEndsWithColon (current_line)){
      current_word =strtok (current_line, ': '); 
      symbol_name = current_word;
      syntax_check_info_print.line_number = line_counter;
      tmp_error_count += SymbolErrorUnite(symbol_name,syntax_check_info_print,macro_list,symbol_table);
      current_line += strlen(current_word)+2;/*skip the colon and space*/
      current_word =strtok(NULL,blank_delimiters);/*TODO check NUll */
      if (0 == strcmp(current_word,".string") ){
        current_line += strlen(current_word)+1;
        if (IsIllegalString(current_line)){/*TODO add the error function!*/
          tmp_error_count++;
        }
        else if (tmp_error_count==0){
          AddSymbol (symbol_table,symbol_name,DC);
          DC += strlen(current_line)-2; /*the length of the string without the apostrophes */
        }
      }
      else if (0 == strcmp(current_word,".data")){
          current_line += strlen(current_word)+1;
          if (CommaIsMissingInData(current_line,syntax_check_info_print)){
                tmp_error_count++;
          }
          if (ParameterIsMissingInData(current_line,syntax_check_info_print)){/*TODO add the error function*/
                tmp_error_count++;
          }
          if (IllegalParametersInData(current_line,syntax_check_info_print)){/*TODO add the error function*/
                tmp_error_count++;
          }
          if (tmp_error_count == 0){
                AddSymbol (symbol_table,symbol_name,DC);
                DC += CountParameters(current_line);
          }
      }
      else if (0 == strcmp(current_word,".extern")){
        current_line += strlen(current_word)+1;
        symbol_name  = strtok (NULL,blank_delimiters);
        tmp_error_count += SymbolErrorUnite(symbol_name,syntax_check_info_print,macro_list,symbol_table);
        if (SymbolAlreadyDefinedAsExtern(symbol_name,symbol_table, syntax_check_info_print)){
          tmp_error_count++;
        } 
        if (DetectExtraCharacters(current_line+strlen (symbol_name),syntax_check_info_print)){
            tmp_error_count++;
        }
        if (tmp_error_count == 0){
            AddExternalSymbol (symbol_table,symbol_name);
            DC += CountParameters(current_line);
        }
      }
      else if (0 == strcmp(current_word,".extern")){}
      else if (FALSE == InstructionDoesntExist(current_word,syntax_check_info_print)){
              if (0 == tmp_error_count){
                  AddSymbol (symbol_table, symbol_name,IC);/*TODO check success*/
              }
      }


      /*handle the rest of the line!*/
      if (tmp_error_count ==0){
        AddSymbol (symbol_table,current_word,address);/*need to understand the address*/
      }
    }
    total_errors += tmp_error_count;
/*
  3. if yes, handle symbol, else:
  4. is it instruction?*/
  }


  /*5. handle instructio
  3. is it directive?
  */
  return 0; //TODO
}

static result_t SecondPass(char *file_path) {
  return 0; //TODO
}
/* @brief - if the first word in a string ends with ':'
*  @param - line: the line
   @return - true if ends with colon, otherwise, false.
*/
static bool_t FirstWordEndsWithColon(char *line){
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
static int CountParameters(char *line){
    int counter = 1;
    while ('\0' != *line){
      line++; 
      if (*line == ','){
        counter++;
      }
    }
    return counter;
}
static int SymbolErrorUnite (char *symbol_name,syntax_check_info_t syntax_check_info_print, macro_table_t *macro_list, symbol_table_t *symbol_table){
      int internal_counter =0;
      if (SymbolPrefixIllegal (symbol_name,syntax_check_info_print)){
          internal_counter++;
      }
      if (SymbolIsIllegal (symbol_name,syntax_check_info_print)){
          internal_counter++;
      }
      if (SymbolExceedCharacterLimit (symbol_name,syntax_check_info_print)){
          internal_counter++;
      }
      if (SymbolExceedCharacterLimit (symbol_name,syntax_check_info_print)){
          internal_counter++;
      }
      if (SymbolUsedAsAMacro (symbol_name, macro_list, syntax_check_info_print)){
          internal_counter++;
      }
      if (SymbolDefinedMoreThanOnce(symbol_name,symbol_table, syntax_check_info_print)){
          internal_counter++;
      }
      return internal_counter;
}
static void HandleSymbol (char *symbol, int line_number){
}