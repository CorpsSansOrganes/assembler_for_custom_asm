#include <stdio.h> /* fopen, fclose */
#include <string.h> 
#include <stdlib.h>
#include "assembler.h"
#include "utils.h"
#include "syntax_errors.h"
#include "macro_table.h"
#include "symbol_table.h"
#include "language_definitions.h"
#include "generate_opcode.h"
#include "vector.h"
#include "list.h"
#include "string_utils.h"
#include "generate_output_files.h"

const char *DELIMITERS = ", \t\n\r";


static bool_t IsSymbolDefinition(char *line);

static bool_t SymbolNameErrorOccurred(const char *symbol_name,
                                  macro_table_t *macro_table,
                                  symbol_table_t *symbol_table,
                                  syntax_check_config_t *cfg);

static result_t FirstPass(char *file_path,
                          macro_table_t *macro_table,
                          symbol_table_t *symbol_table,
                          vector_t *code_table,
                          vector_t *data_table);

static result_t SecondPass(char *file_path,
                           symbol_table_t *symbol_table,
                          vector_t *code_table,
                          vector_t *data_table,
                           list_t *ext_symbol_occurrences);

static result_t UpdateAdresses(symbol_table_t *symbol_table,
                               int IC);

static int SplitOperands(char *line,
                         operand_t *first_operand,
                         operand_t *second_operand );

static int ExternalSymbolCompare(external_symbol_data_t *external_symbol_data,
                                 char *key);

static int CountParameters(char *line);

static result_t HandleDirectiveStatement();

static result_t HandleStringOrData(bool_t is_string,
                                   char *param,
                                   symbol_table_t *symbol_table,
                                   char *symbol_name,
                                   vector_t *data_table,
                                   syntax_check_config_t *cfg);


result_t AssembleFile(char *file_path, macro_table_t *macro_table) {
  bool_t no_errors = TRUE;

  /* Symbol table which will be populated with symbols in first pass */
  symbol_table_t *symbol_table;

  /* Two vectors for holding machine code for code & data segments, respectively. */
  vector_t *code_table = NULL;
  vector_t *data_table = NULL;

  /* List which will holds all places where an external symbol is used */
  list_t *ext_symbol_occurrences = CreateList();

  /*
   * Acquiring resources 
   */

  if (NULL == ext_symbol_occurrences) {
    fprintf(stderr,"Memory allocation error: couldn't allocate ext. symbol usage list\n");
    return MEM_ALLOCATION_ERROR;
  }

  symbol_table = CreateSymbolTable();
  if (NULL == symbol_table) {
    fprintf(stderr,"Memory allocation error: couldn't allocate a symbol table\n");
    DestroyList(ext_symbol_occurrences);
    return MEM_ALLOCATION_ERROR;
  }  

  code_table = CreateVector(10, sizeof(vector_t *));
  if (NULL == code_table) {
    fprintf(stderr,"Memory allocation error: couldn't allocate a code table\n");
    DestroyList(ext_symbol_occurrences);
    DestroySymbolTable(symbol_table);
    return MEM_ALLOCATION_ERROR;
  }  

  data_table = CreateVector(10, sizeof(vector_t *));
  if (NULL == data_table) {
    fprintf(stderr,"Memory allocation error: couldn't allocate a data table\n");
    DestroyList(ext_symbol_occurrences);
    DestroySymbolTable(symbol_table);
    DestroyVector(code_table);
    return MEM_ALLOCATION_ERROR;
  }  

  /*
   * Assembler performing first & second pass
   */
  if (SUCCESS != FirstPass(file_path,
                           macro_table,
                           symbol_table,
                           code_table, 
                           data_table)) {
    no_errors = FALSE;
  }

  if (no_errors && 
    SUCCESS != SecondPass(file_path,
                          symbol_table,
                          code_table,
                          data_table,
                          ext_symbol_occurrences)) {
    no_errors = FALSE;

  }

  /*
   * Generating output files 
   */
  if (no_errors &&
      SUCCESS != GenerateOutputFiles(code_table,
                                     data_table,
                                     symbol_table,
                                     file_path,
                                     ext_symbol_occurrences)) {
    no_errors = FALSE;
  }

    DestroyList(ext_symbol_occurrences);
    DestroySymbolTable(symbol_table);
    DestroyVector(code_table);
    DestroyVector(data_table);
    return no_errors ? SUCCESS : FAILURE;

}

/*
 * @brief Performs a first pass on the code:
 *        1. Creating symbol table from symbol declarations.
 *        2. Creating initial memory mapping, which will be completed in the
 *           second pass.
 *
 * @param file_path - The file to read & perform first pass on.
 *
 *        macro_table - Table of all macros identified in the preprocessing.
 *
 *        symbol_table - A valid empty symbol table object, which will be populated
 *                       during first pass.
 *
 *        code_table - A valid empty vector which will contain the machine code
 *                     words corresponding to the assembly instruction statements.
 *        
 *        data_table - A valid empty vector which will contain the machine code
 *                     encoding of directive statements.
 *
 * @return SUCCESS if no syntax error or other fault occurred.
 *         FAILURE if one or more syntax errors occurred.
 *         MEM_ALLOCATION_ERROR if a memory allocataion error occurred.
 */

static result_t FirstPass(char *file_path,
                          macro_table_t *macro_table,
                          symbol_table_t *symbol_table,
                          vector_t *code_table,
                          vector_t *data_table) {
  int total_errors = 0;
  char *current_word = NULL; 
  char *current_line = NULL;
  char *symbol_name = NULL;
  FILE *input_file = NULL;
  operand_t *first_operand = NULL;
  operand_t *second_operand = NULL;
  int num_of_operands;
  int num_of_parameters;
  syntax_check_config_t cfg = CreateSyntaxCheckConfig (file_path, 0, TRUE);

  /* Acquire resources */
  input_file = fopen(file_path, "r");
  if (NULL == input_file) {
    fprintf(stderr,"Couldn't open input file");
    return ERROR_OPENING_FILE; 
  }

  current_line = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
  if (NULL == current_line) {
    fclose (input_file);
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;
  }


  /* ~ * ~ ------------------------------ ~ * ~
   * Performing syntax analysis for each line.
   * ~ * ~ ------------------------------ ~ * ~
   */
  while (NULL != fgets(current_line, MAX_LINE_LENGTH, input_file)) {
    ++cfg.line_number;
    symbol_name = NULL;

    /* 
     * Does the line begin with a symbol definition?
     * e.g. "SYMBOL: ..."
     */
    if (IsSymbolDefinition(current_line)) {
      /* Extract symbol name */
      current_word = strtok(current_line, ": \t\n\r");
      symbol_name = StrDup(current_word);

      if (NULL == symbol_name) {
        // TODO - handle mem error.
      }
      
      if (SymbolNameErrorOccurred(symbol_name,
                                  macro_table,
                                  symbol_table,
                                  &cfg)) {
        total_errors++;
        continue;
      }

      /* Skip the colon and space*/
      current_line += strlen(current_word) + 2; 
      if (NoDefinitionForSymbol(current_line, &cfg)){
        total_errors++;
        continue;
      }

      /* Next word after symbol definition */
      current_word = strtok(NULL, DELIMITERS);
    }        

    else {
      /* First word (no symbol definition) */
      current_word = strtok(current_line,DELIMITERS);
    }

    if ('.' == current_word[0]) {
      // Handle directives
      if (MEM_ALLOCATION_ERROR == HandleDirectiveStatement(current_word)) {

      }
    }


    /*instruction exist*/
    else if (FALSE == InstructionDoesntExist(current_word,&cfg)){
          if (symbol_name[0] != '\0'){
           if (SUCCESS != AddSymbol (symbol_table, symbol_name,*IC+INITIAL_IC_VALUE, CODE)){
            fclose (input_file);
            free (current_line);
            free (current_word);
            free (symbol_name);
             return FAILURE;
           }
          }
          current_line += strlen (current_word)+1;
          num_of_operands = SplitOperands (current_line, first_operand,second_operand);

          if (ImmediateOperandTooBig(first_operand,&cfg)){
            total_errors++;
            continue;
          }
          if (ImmediateOperandTooBig(second_operand,&cfg)){
            total_errors++;
            continue;
          }
          if (WrongNumberOfOperands(current_word,num_of_operands,&cfg)){
            total_errors++;
            continue;
          }
          if (IncorrectAddressingMethod(current_word,first_operand,&cfg)){
            total_errors++;
            continue;
          }
          if (IncorrectAddressingMethod(current_word,second_operand,&cfg)){
            total_errors++;
            continue;
          }
            VectorAppend (opcode, InstructionLineToMachineCode (first_operand, second_operand, num_of_operands, current_word, IC));
    }
    /*instruction doesnt exist*/
    else {
      total_errors++;
      continue;
    }
  }

  free(current_line);
  free(current_word);
  fclose(input_file);

  if (0 == total_errors){
    if (SUCCESS != UpdateAdresses (symbol_table, *IC)){
      return FAILURE;
  }
    return SUCCESS;
  }
  return FAILURE;
}

static result_t SecondPass(char *file_path, symbol_table_t *symbol_table, vector_t *opcode, list_t *ext_symbol_occurrences){
  syntax_check_config_t syntax_check_config_print = CreateSyntaxCheckConfig (file_path,
                                                 0,
                                                 TRUE);
  int line_counter = 0;
  symbol_t *symbol = NULL;
  int total_errors = 0;
  int vector_counter = 0;
  int bitmap_counter = 0;/*counts the overall number of bitmaps*/
  bitmap_t *opcode_bitmap; 
  char *current_word = NULL; 
  char *current_line = NULL;
  char *symbol_name=NULL;
  FILE *input_file = NULL;
  external_symbol_data_t *external_symbol_data;
  
  /*allocate resources*/
  input_file = fopen(file_path, "r");
    if (NULL == input_file) {
    fprintf(stderr,"Couldn't open input file");
    return ERROR_OPENING_FILE; 
  }

  current_line = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
  if (NULL == current_line) {
    fclose (input_file);
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;
  }
  current_word = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
  if (NULL == current_word) {
    fclose (input_file);
    free (current_line);
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;
  }

  char *symbol_name = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
  if (NULL == symbol_name) {
    fclose (input_file);
    free (current_line);
    free (current_word);
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;
  }
  /*for each line in the input*/
  while (NULL != fgets(current_line, MAX_LINE_LENGTH, input_file)) {
    line_counter++;
    symbol_name = '/0';
    vector_t *opcode_line;
    if (IsSymbolDefinition(current_line)) {
        current_word = strtok (current_line, DELIMITERS);
        current_line += strlen (current_word) +1;               
    }
    current_word = strtok (current_line, DELIMITERS);
    current_line += strlen (current_word) +1;      
    if (0 == strcmp (current_word, ".extern") || 0 == strcmp (current_word, ".data")){
        /*nothing to do, continue to the next line*/
    }
     if (0 == strcmp (current_word, ".string")){
        vector_counter += strlen (current_line)-2; /*the length of the string without the quotation marks pl*/
        bitmap_counter += strlen (current_line)-2; 
     }
    if (0 == strcmp (current_word, ".entry")){
       current_line += strlen (current_word) +1; 
       if (IsIllegalExternOrEntryParameter(current_line,&syntax_check_config_print)){
          total_errors++;
          continue;
       }
       current_word = strtok (current_line,DELIMITERS);
       while (NULL != current_line){
          if (SymbolWasntDefined(current_word, symbol_table, &syntax_check_config_print)){
            total_errors++;
            continue;
          }
          if (SymbolAlreadyDefinedAsExtern(current_word,symbol_table,&syntax_check_config_print)){
            total_errors++;
            continue;
          }
          if (ChangeSymbolToEntry(symbol_table,current_word) != SUCCESS){
            fclose (input_file);
            free (current_line);
            free (current_word);
            free (symbol_name);
            return FAILURE;
         }
          current_word = strtok (NULL,DELIMITERS);

       }


    }
    else if (FALSE == InstructionDoesntExist(current_word,&syntax_check_config_print)){/* silent config?*/
        current_word = strtok (NULL,DELIMITERS);
        if (NULL != current_word){
          symbol = FindSymbol (symbol_table,current_word);
          if (NULL != symbol){/*first operand is a symbol*/
            opcode_line = GetElementVector (opcode, vector_counter);
            opcode_bitmap = GetElementVector (opcode_line,1);/*find his bitmap*/
            *opcode_bitmap = GetSymbolAddress (symbol);/*put the address*/
            if (EXTERN == GetSymbolType (symbol)){/*if its extern add the occurence to the list for the .ext file*/
               external_symbol_data = Find(ext_symbol_occurrences, ExternalSymbolCompare, GetSymbolName(symbol));
               if (NULL == external_symbol_data){/*if thats the first occurence*/
                external_symbol_data->symbol_name = GetSymbolName(symbol);
                external_symbol_data->occurences = CreateVector (0,sizeof(int));
                AddNode (ext_symbol_occurrences,external_symbol_data);
               }
               AppendVector (external_symbol_data->occurences,bitmap_counter+INITIAL_IC_VALUE+1);/*bit_map counts the memory words that been used, so thats give the address*/

            }
          } 
        }
        current_word = strtok (NULL,DELIMITERS);
          if (NULL != current_word){
            symbol = FindSymbol (symbol_table,current_word);
            if (NULL != symbol){
              opcode_line = GetElementVector (opcode, vector_counter);
              opcode_bitmap = GetElementVector (opcode_line,2);
              *opcode_bitmap = GetSymbolAddress (symbol);
            } 
            if (EXTERN == GetSymbolType (symbol)){/*if its extern add the occurence to the list for the .ext file*/
               external_symbol_data = Find(ext_symbol_occurrences, ExternalSymbolCompare, GetSymbolName(symbol));
               if (NULL == external_symbol_data){/*if thats the first occurence*/
                external_symbol_data->symbol_name = GetSymbolName(symbol);
                external_symbol_data->occurences = CreateVector (0,sizeof(int));
                AddNode (ext_symbol_occurrences,external_symbol_data);
               }
               AppendVector (external_symbol_data->occurences,bitmap_counter+INITIAL_IC_VALUE+2);/*bit_map counts the memory words that been used, so thats give the address*/
            }
        }
        bitmap_counter += GetCapacityVector( GetElementVector(opcode,vector_counter));/*adds the number of bitmaps in the current element*/
        vector_counter++;
    }
  

  }
  fclose (input_file);
  free (current_line);
  free (current_word);
  free (symbol_name);
  if (0 == total_errors){
    return SUCCESS;
  }
  return FAILURE;
}

/* @brief - if the first word in a string ends with ':'
*  @param - line: the line
   @return - true if ends with colon, otherwise, false.
*/
static bool_t IsSymbolDefinition(char *line) {
    while (!IsBlank(*(line + 1))) {
       line++;
    }
    if (*line == ':'){
	      return TRUE;
    }
    return FALSE;
}

/*
 * @brief Checks all possible syntax errors of symbols.
 *
 * @param symbol_name - The name of the symbol, e.g. "SYMBOL".
 *        macro_table - Macro table of the file.
 *        symbol_table - Symbol table of the file.
 *        cfg - Syntax error configurations, see in syntax_errors.h.
*/

static bool_t SymbolNameErrorOccurred(const char *symbol_name,
                                  macro_table_t *macro_table,
                                  symbol_table_t *symbol_table,
                                  syntax_check_config_t *cfg) {
  int internal_counter = 0;
  if (SymbolNameIsIllegal(symbol_name,cfg)) {
    internal_counter++;
  }
  if (SymbolUsedAsAMacro(symbol_name, macro_table, cfg)) {
    internal_counter++;
  }
  if (SymbolDefinedMoreThanOnce(symbol_name,symbol_table, cfg)) {
    internal_counter++;
  }

  return internal_counter ? TRUE : FALSE;
}

/* @brief - generating the opcode of an operand, according to its adressing method.
*           if its addressing is Direct (which means it is a symbol) 
*           nothing it will return 0, and it will be computed in the second pass
*  @param - operand: the operand
*  @return - the opcode of the operand
*/


static result_t UpdateAdresses (symbol_table_t *symbol_table, int IC){
  list_t *symbol_list =  AsList(symbol_table);
  symbol_t *symbol = (symbol_t *) GetHead (symbol_list);
  int address;
  if (NULL != symbol){
    address = GetSymbolAddress (symbol);
  }
  while (NULL != symbol){
    if (DATA == GetSymbolAddress(symbol)){
      UpdateSymbolAddress (symbol,address+IC+INITIAL_IC_VALUE);/* maybe its possible like that? symbol->address += IC+INITIAL_IC_VALUE;*/
    }
    symbol = (symbol_t *) GetNext (symbol);
  }
  return SUCCESS;
}

static int SplitOperands(char *line, operand_t *first_operand, operand_t *second_operand) {
    int counter = 0;
    char *current_word = strtok (line,DELIMITERS);
    if (current_word != NULL){
      first_operand->name = StrDup (current_word);
      first_operand->addressing_method =DetectAddressingMethod(first_operand->name);
      first_operand->type = DESTINATION_OPERAND;
      counter++;
    }
    current_word = strtok (NULL, DELIMITERS);
    if (current_word != NULL){
      second_operand->name =StrDup (current_word);
      second_operand->addressing_method =DetectAddressingMethod(second_operand->name);
      second_operand->type = DESTINATION_OPERAND;
      first_operand->type = SOURCE_OPERAND;
      counter++;
      while ('\0' != *current_word) {
      if (*line == ','){
        counter++;
      }
      current_word++;
    }
    }
    return counter;
  }  
static int ExternalSymbolCompare (external_symbol_data_t *external_symbol_data, char *key) {
  return (0 == strcmp (external_symbol_data->symbol_name, key));
}

static result_t HandleDirectiveStatement(char *current_word, syntax_check_config_t *cfg) {
  /*
   * First, check if that directive even exists
   */
  if (DirectiveDoesntExist(current_word, cfg)) {
    return FAILURE;
  }
  
  /*
   * Is it a .string directive?
   * e.g. "SYMBOL: .string ..."
   *      ".string ..."
   */
  else if (0 == strcmp(current_word,".string")) {
      result_t res = HandleStringOrData(
        ".string", current_line, 
        IsIllegalString, symbol_table,
        symbol_name, StringDirectiveToMachinecode,
        data_table, &cfg
      );

      if (MEM_ALLOCATION_ERROR == res) {
        fclose(input_file);
        free(current_line);
        if (symbol_name) {
          free(symbol_name);
        }
        return MEM_ALLOCATION_ERROR;
      }

      else if (FAILURE == res) {
        total_errors++;
        continue; 
      }
    }  

    /*
     * Is it a .data directive?
     * e.g. "SYMBOL: .data ..."
     *      ".data ..."
     */
    else if (0 == strcmp(current_word,".data")) {
      result_t res = HandleStringOrData(
        ".data", current_line, 
        IsIllegalDataParameter, symbol_table,
        symbol_name, StringDirectiveToMachinecode,
        data_table, &cfg
      );

      if (MEM_ALLOCATION_ERROR == res) {
        fclose(input_file);
        free(current_line);
        if (symbol_name) {
          free(symbol_name);
        }
        return MEM_ALLOCATION_ERROR;
      }

      else if (FAILURE == res) {
        total_errors++;
        continue; 
      }
    }  

     /*
     * Is it a .extern directive?
     * e.g. "SYMBOL: .extern..."
     *      ".data ..."
     */
    else if (0 == strcmp(current_word,".extern")) {
      current_line += strlen(current_word) + 1;
      if (IsIllegalExternOrEntryParameter(current_line, &cfg)){
        total_errors++;
        continue;
      }

      /* Label before .extern warns warning */
      if (NULL != symbol_name) {
        // TODO warning
        free(symbol_name);
      }

      symbol_name = strtok (NULL,DELIMITERS);
      while (NULL != symbol_name){/*for every symbol in the line*/
        if (0 < SymbolNameErrorOccurred(symbol_name,&cfg,macro_table,symbol_table)){
          total_errors++;
           continue;
        }
        if (SymbolAlreadyDefinedAsEntry(symbol_name,symbol_table, &cfg)){
          total_errors++;
           continue;
        }
        if (SUCCESS != AddExternalSymbol(symbol_table, symbol_name)){
          fclose (input_file);
          free (current_line);
          free (current_word);
          free (symbol_name);
          return FAILURE;
        }
        symbol_name = strtok (NULL,DELIMITERS);
      }
    }
    else if (0 == strcmp(current_word,".entry")) {
      /* Do nothing: will handle in 2nd pass */
    }
}

static result_t HandleStringOrData(bool_t is_string,
                                   char *param,
                                   symbol_table_t *symbol_table,
                                   char *symbol_name,
                                   vector_t *data_table,
                                   syntax_check_config_t *cfg) {

  typedef bool_t (*syntax_check_func_t)(const char *, syntax_check_config_t *);
  typedef result_t (*machine_code_generation_func_t)(vector_t *, char *);

  const char *directive = NULL;
  syntax_check_func_t syntax_check_func = NULL;
  machine_code_generation_func_t generate_machine_code = NULL;

  if (is_string) {
    directive = ".string";
    syntax_check_func = IsIllegalString;
    generate_machine_code = StringDirectiveToMachinecode;
  }
  else { /* .data */
    directive = ".data";
    syntax_check_func = IsIllegalDataParameter;
    generate_machine_code = DataDirectiveToMachinecode;
  }

  /* Skip to directive's parameter */
  param += strlen(directive) + 1;
  
  /* Check syntax errors in parameters */
  if (syntax_check_func(param, cfg)) {
    return FAILURE; 
  }

  /* Create symbol, if one was defined */
  if (NULL != symbol_name) {
    if (SUCCESS != AddSymbol(symbol_table,
                             symbol_name,
                             GetSizeVector(data_table),
                             DATA)) {

      return MEM_ALLOCATION_ERROR;
    }
  }

  /* Generate machine code in the data segment */
  if (SUCCESS != generate_machine_code(data_table, param)) {
    return MEM_ALLOCATION_ERROR;
  }

  return SUCCESS;
}
