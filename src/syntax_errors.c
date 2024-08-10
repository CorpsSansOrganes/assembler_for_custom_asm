#include <string.h> /* strchr */
#include <ctype.h> /* isblank */
#include <stdio.h>
#include "syntax_errors.h"
#include "utils.h"
#include "symbol_table.h"
#include "language_definitions.h"

struct syntax_check_config {
  const char *file_name;
  unsigned int line_number;
  bool_t verbose;
};

/*
*@brief detects which type of addressing method fit to the operand, and returns invalid if neccessary.
*@param operand - the operand
*@return the type of addressing method for the operand
*/
static addressing_methods detect_addressing_method (char *operand);
static char *string_tolower(char *str);
/*static int SymbolCompare (void *str,void *key );*/

/* This config is used for calling other syntax checks internally silently */
syntax_check_config_t silent_syntax_cfg = {NULL, 0, FALSE};

syntax_check_config_t CreateSyntaxCheckConfig(const char *file_name,
                                              unsigned int line_number,                                          
                                              bool_t verbose) {
  syntax_check_config_t info;
  info.file_name = file_name;
  info.line_number = line_number;
  info.verbose = verbose;

  return info;
}

bool_t DetectExtraCharacters(const char *starting_from,
                             syntax_check_config_t *config) {
  char *ptr = (char *)starting_from;

  while (isblank(*ptr)) {
    ++ptr;
  }

  if ('\0' != *ptr) {
    if (config->verbose) {
      printf ("Error: extraneous characters detected at line %u in file: %s\n",
              config->line_number, config->file_name);
    }
    return TRUE;
  }

  return FALSE;
}

bool_t IsReservedName(const char *name, syntax_check_config_t *config) {
  if (FALSE == InstructionDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf ("ERROR: attempt to make use of a reserved instruction name '%s' at line %u in file: %s\n",
              name,
              config->line_number,
              config->file_name);
    }
    return TRUE;
  }

  if (FALSE == DirectiveDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf ("ERROR: attempt to make use of a reserved directive name '%s' at line %u in file: %s\n",
              name,
              config->line_number, 
              config->file_name);
    }
    return TRUE;
  }

  if (FALSE == RegisterNameDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf ("ERROR: attempt to make use of a reserved register name '%s' at line %u in file: %s\n",
              name,
              config->line_number, 
              config->file_name);
    }
    return TRUE;
  }

  return FALSE;
}

bool_t InstructionDoesntExist(const char *instruction,
                              syntax_check_config_t *config) {
   int i;
   for (i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
       if (0 == strcmp(instruction, reserved_instructions[i])) {
           return FALSE;
       }
    }

    if (config->verbose){
        printf ("ERROR: the instruction '%s' at line %u in the file %s doesn't exist",
                instruction,
                config->line_number,
                config->file_name);
    }
    return TRUE;
}

int WrongNumberOfOperands(const char *instruction,
                          int num_of_operands,
                          syntax_check_config_t *config) {
  int i = 0;
  int required_operands = 1;

  while (0 != strcmp(address_method_table[i].name, instruction)) {
    ++i;
  }
  required_operands = (address_method_table[i].is_there_destination_operand
                          + address_method_table[i].is_there_source_operand);

  if (num_of_operands != required_operands) {
    if (config->verbose) {
      printf ("Error: expected %d operands, but given %d at line %u in the file %s",
              required_operands,
              num_of_operands,
              config->line_number,
              config->file_name);
    }
  }

  return num_of_operands - required_operands;
}

bool_t IncorrectAddressingMethod(const char *instruction,
                                 const char *operand,
                                 operand_type_t type,
                                 syntax_check_config_t *config) {
    int i = 0;
    int dummy = 100;
    int method = (int) detect_addressing_method (operand);
        for (i=0; i<NUM_OF_INSTRUCTIONS;i++){
            if (*address_method_table[i].name == *instruction){
                if (type == SOURCE_OPERAND){
                    if (!address_method_table[i].source_operand_methods[method]){
                        if(syntax_check_config.verbose){
                          printf ("Error: the addressing method of the operand %s at line %u in the file: %s is not compatible to the instruction", operand, syntax_check_config.line_number, syntax_check_config.file_name);
                        }
                        return TRUE;  
                    }
                    return FALSE;
                }
                else if (type == DESTINATION_OPERAND){
                    if (!address_method_table[i].destination_operand_methods[method]){
                        if(syntax_check_config.verbose){
                          printf ("Error: the addressing method of the operand %s at line %u in the file: %s is not compatible to the instruction", operand, syntax_check_config.line_number, syntax_check_config.file_name);
                        }
                        return TRUE;
                    }
                    return FALSE;
                }
            }
        }
    return dummy;
}

bool_t SymbolDefinedMoreThanOnce(char *symbol, symbol_table_t *table, syntax_check_config_t syntax_check_config){
   if (NULL != FindSymbol (table, symbol)){
	if (syntax_check_config.verbose){
        printf ("Error: attempted to define a symbol at line %u in the file: %s that already been defined", syntax_check_config.line_number, syntax_check_config.file_name);
    }
    return TRUE;
   }
   return FALSE;
}

bool_t SymbolWasntDefined(char *symbol, symbol_table_t *table, syntax_check_config_t syntax_check_config){
   if (NULL != FindSymbol (table, symbol)){
	return FALSE;
   }
    if (syntax_check_config.verbose){
        printf ("Error: attempted to call a symbol at line %u in the file: %s that wasn't defined", syntax_check_config.line_number, syntax_check_config.file_name);
    }
   return TRUE;
}

bool_t ColonSyntaxError(char *symbol, syntax_check_config_t syntax_check_config){
    while (*(symbol+1) != '\0'){
       symbol++;
    }
    if (*symbol != ':'){
        if (syntax_check_config.verbose){
        printf ("Error: symbol was used without colon or with space between the symbol and the colon at line %u in the file: %s", syntax_check_config.line_number, syntax_check_config.file_name);
        }
	return TRUE;
    }
    return FALSE;
}


bool_t SymbolAlreadyDefinedAsEntry(char *symbol_name, symbol_table_t *table, syntax_check_config_t syntax_check_config){
    symbol_t *extern_symbol = FindSymbol(table,symbol_name);
    if (GetSymbolType(extern_symbol) == ENTRY){
        if (syntax_check_config.verbose){
            printf ("ERROR: attempt to define an entry symbol at line %u in the file: %s as an extern", syntax_check_config.line_number, syntax_check_config.file_name);
        }
        return TRUE;
    }
    return FALSE;
}

bool_t SymbolAlreadyDefinedAsExtern(char *symbol_name, symbol_table_t *table, syntax_check_config_t syntax_check_config){
    symbol_t *entry_symbol = FindSymbol(table,symbol_name);
    if (GetSymbolType(entry_symbol) == EXTERN){
        if (syntax_check_config.verbose){
            printf ("ERROR: attempt to define an extern symbol at line %u in the file: %s as an entry", syntax_check_config.line_number, syntax_check_config.file_name);
        }
        return TRUE;
    }
    return FALSE;
}

bool_t SymbolIsIllegal(char *symbol, syntax_check_config_t syntax_check_config) {
    int i;
    for (i = 0; i < strlen(symbol); i++)
     {
        if (!((*symbol >= 'A' && *symbol <= 'Z') || (*symbol >= 'a' && *symbol <= 'z') || (*symbol >= '0' && *symbol <= '9'))) {
            if (syntax_check_config.verbose){
                printf ("ERROR: use of illegal characters in the symbol name at line %u in the file: %s", syntax_check_config.line_number, syntax_check_config.file_name);
            }
            return TRUE;
        }
        symbol++;
    }

    return FALSE;
}

bool_t SymbolPrefixIllegal(char *symbol, syntax_check_config_t syntax_check_config) {

    if (!((*symbol >= 'A' && *symbol <= 'Z') || (*symbol >= 'a' && *symbol <= 'z'))) {
        if (syntax_check_config.verbose){
            printf ("ERROR: illegal character in the prefix of a symbol at line %u in the file: %s", syntax_check_config.line_number, syntax_check_config.file_name);
        }
        return TRUE;
    }

    return FALSE;
}


bool_t RegisterNameDoesntExist(char *register_name, syntax_check_config_t syntax_check_config){
    int i;
    for (i = 0; i < NUM_OF_REGISTERS; i++) {
        if (strcmp(register_name, register_names[i]) == 0) {
            return FALSE;
        }
    }
    if (syntax_check_config.verbose){
        printf ("ERROR: the register name at line %u in the file: %s doesn't exist", syntax_check_config.line_number, syntax_check_config.file_name);
    }
    return TRUE;
}

bool_t DirectiveDoesntExist(char *directive, syntax_check_config_t syntax_check_config) {
    int i;
    for (i = 0; i < NUM_OF_DIRECTIVES; i++) {
        if (strcmp(directive, reserved_directives[i]) == 0) {
            return FALSE;
        }
    }
    if (syntax_check_config.verbose){
        printf ("ERROR: the directive name at line %u in the file: %s doesn't exist", syntax_check_config.line_number, syntax_check_config.file_name);
    }
    return TRUE;
}
bool_t DirectiveIsUpperCase(char *directive, syntax_check_config_t syntax_check_config){
    if (DirectiveDoesntExist(directive, silent_syntax_cfg) && !DirectiveDoesntExist(string_tolower(directive), silent_syntax_cfg))
    {
        if (syntax_check_config.verbose){
            printf ("ERROR: directive was written in uppercase letters instead of lowercase letters at line: %u in the file: %s doesn't exist", syntax_check_config.line_number, syntax_check_config.file_name);
        }
        return TRUE;
    }
    return FALSE;
}

bool_t SymbolExceedCharacterLimit(char *symbol, syntax_check_config_t syntax_check_config) {
    int length = 0;
    if (symbol == NULL) {
        return FALSE;
    }
    while (*symbol != '\0') {
        length++;
        if (length > 31) {
            if (syntax_check_config.verbose){
                printf ("ERROR: the name of the symbol at line %u in the file: %s exceeded the character limit", syntax_check_config.line_number, syntax_check_config.file_name);
            }
            return TRUE;
        }
        symbol++;
    }

    return FALSE;
}
bool_t SymbolUsedAsAMacro(char *symbol, macro_table_t *macro_list, syntax_check_config_t syntax_check_config){
   if (NULL != FindMacro( macro_list, symbol)){
	if (syntax_check_config.verbose){
        printf ("ERROR: attempt to define a symbol name that is already define as a macro at line %u in the file: %s", syntax_check_config.line_number, syntax_check_config.file_name);
    }
    return TRUE;
   }  
   return FALSE;
}

bool_t CommaIsMissingInData(char *data, syntax_check_config_t syntax_check_config){
   while (*data != '\0') {
	 if (*data == ' ' || *data == '\t' ){
		if (*(data-1) != ','){
			if (syntax_check_config.verbose){
                printf ("ERROR: a comma is missing between parameters at line %u in the file: %s", syntax_check_config.line_number, syntax_check_config.file_name);
            }
            return TRUE;
        }
     }
   }
   return FALSE;
}

/*static int SymbolCompare (void *str,void *key ){
   return (0 == strcmp ((char *) str, (char *) key ));  
}*/
static char *string_tolower(char *str) {
    int i;
    int len = strlen(str);
    for (i = 0; i < len; i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}
static addressing_methods detect_addressing_method (char *operand){
    int i;
    if (*operand == '#'){
        operand++;
        if (*operand == '-' || *operand == '+'){
            operand++;
        }
        if (strlen(operand)==0){
            return INVALID;
        }
        for (i = 0; i < strlen(operand); i++)
        {
           if ( ! isdigit (*operand+i))
           {
            return INVALID;
           }
        }
        return IMMEDIATE;                
    }
    if (*operand == '*'){
        if (RegisterNameDoesntExist(operand+1,silent_syntax_cfg)){
            return INVALID;
        }
        return INDIRECT_REGISTER;
    }
    if (!RegisterNameDoesntExist(operand,silent_syntax_cfg)){
        return DIRECT_REGISTER;
    }
    return DIRECT;
}
