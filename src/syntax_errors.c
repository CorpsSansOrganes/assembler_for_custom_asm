#include <string.h> /* strchr */
#include <ctype.h> /* isblank, isalpha */
#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free */
#include "syntax_errors.h"
#include "utils.h"
#include "symbol_table.h"
#include "language_definitions.h"

static addressing_method_t DetectAddressingMethod(const char *operand);

static bool_t TakesOperand(instruction_t instruction, operand_type_t type);

static bool_t AddressingMethodIsLegal(instruction_t instruction,
                                      operand_type_t type,
                                      addressing_method_t method);

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

  if ('\0' == *ptr) {
    return FALSE;
  }

  else if (config->verbose) {
    printf ("Error: Extraneous characters detected at line %u in file %s\n",
            config->line_number, config->file_name);
  }
  return TRUE;
}

bool_t IsReservedName(const char *name, syntax_check_config_t *config) {
  if (FALSE == InstructionDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf ("ERROR: attempt to make use of a reserved instruction name '%s' at line %u in file %s\n",
              name,
              config->line_number,
              config->file_name);
    }
    return TRUE;
  }

  if (FALSE == DirectiveDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf ("ERROR: attempt to make use of a reserved directive name '%s' at line %u in file %s\n",
              name,
              config->line_number, 
              config->file_name);
    }
    return TRUE;
  }

  if (FALSE == RegisterNameDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf ("ERROR: attempt to make use of a reserved register name '%s' at line %u in file %s\n",
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
  int i = 0;

  for (i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
    if (0 == strcmp(instruction, reserved_instructions[i].name)) {
      return FALSE;
    }
  }
  
  if (config->verbose) {
    printf ("ERROR: the instruction '%s' at line %u in the file %s doesn't exist\n",
            instruction,
            config->line_number,
            config->file_name);
  }
  return TRUE;
}

bool_t WrongNumberOfOperands(const char *instruction,
                          int num_of_operands,
                          syntax_check_config_t *config) {
  int i = 0;
  int required_operands = 1;

  while (0 != strcmp(reserved_instructions[i].name, instruction)) {
    ++i;
  }

  required_operands = (TakesOperand(reserved_instructions[i], SOURCE_OPERAND) 
                      + TakesOperand(reserved_instructions[i], DESTINATION_OPERAND));

  if (num_of_operands == required_operands) {
    return FALSE;
  }

  if (config->verbose) {
    printf ("Error: Expected %d operands, but given %d at line %u in the file %s\n",
            required_operands,
            num_of_operands,
            config->line_number,
            config->file_name);
  }
  return TRUE;
}

bool_t IncorrectAddressingMethod(const char *instruction,
                                 const operand_t *operand,
                                 operand_type_t type,
                                 syntax_check_config_t *config) {
  int i = 0;
  addressing_method_t method = operand->addressing_method;
  while (0 != strcmp(reserved_instructions[i].name, instruction)) {
    ++i;
  }

  if (TRUE == AddressingMethodIsLegal(reserved_instructions[i], type, method)) {
    return FALSE;
  }

  if (config->verbose) {
    printf("Error: Addressing method of the operand %s at line %u in file %s is illegal for the instruction '%s'\n",
           operand->name, 
           config->line_number,
           config->file_name,
           instruction);
  }
  return TRUE;
}

bool_t SymbolDefinedMoreThanOnce(char *symbol,
                                 symbol_table_t *table,
                                 syntax_check_config_t *config) {

  if (TRUE == SymbolWasntDefined(symbol, table, &silent_syntax_cfg)) {
    return FALSE;
  }

  if (config->verbose) {
    printf ("Error: Attempted to define a symbol at line %u in the file %s that already been defined\n",
            config->line_number,
            config->file_name);
  }
  return TRUE;
}

bool_t SymbolWasntDefined(char *symbol,
                          symbol_table_t *table,
                          syntax_check_config_t *config){
  if (NULL != FindSymbol (table, symbol)) {
    return FALSE;
  }

  if (config->verbose) {
    printf ("Error: Attempted to call a symbol at line %u in file %s that wasn't defined\n",
            config->line_number,
            config->file_name);
  }
  return TRUE;
}


bool_t SymbolAlreadyDefinedAsEntry(char *symbol_name,
                                   symbol_table_t *table,
                                   syntax_check_config_t *config) {
  symbol_t *extern_symbol = FindSymbol(table,symbol_name);
  if (ENTRY != GetSymbolType(extern_symbol)) {
    return FALSE;
  }

  if (config->verbose) { 
    printf ("ERROR: Attempt to define an entry symbol at line %u in file %s as an extern\n",
            config->line_number,
            config->file_name);
  }
  return TRUE;
}

bool_t SymbolAlreadyDefinedAsExtern(char *symbol_name,
                                    symbol_table_t *table,
                                    syntax_check_config_t *config) {

  symbol_t *entry_symbol = FindSymbol(table,symbol_name);
  if (EXTERN != GetSymbolType(entry_symbol)) {
    return FALSE;
  }

  if (config->verbose){
    printf ("ERROR: Attempt to define an extern symbol at line %u in the file %s as an entry\n",
            config->line_number,
            config->file_name);
  }
  return TRUE;
}

bool_t SymbolNameIsIllegal(const char *symbol, syntax_check_config_t *config) {
  char *ptr = (char *)symbol;

  while ('\0' != *ptr && isalpha(*ptr)) {
    ++ptr;
  }

  if ('\0' == *ptr) {
    return FALSE;
  }

  if (config->verbose) {
    printf ("ERROR: Use of illegal characters in the symbol name '%s' at line %u in file %s\n",
            symbol,
            config->line_number,
            config->file_name);
  }
  return TRUE;
}

bool_t SymbolPrefixIllegal(const char *symbol, syntax_check_config_t *config) {
  if (isalpha(*symbol)) {
    return FALSE;
  }

  return TRUE;
}


bool_t SymbolExceedCharacterLimit(const char *symbol,
                                  syntax_check_config_t *config) {
  unsigned int length = 0;

  while ('\0' != *symbol && length <= 31) {
    length++;
    symbol++;
  }

  if (length <= 31) {
    return FALSE;
  }

  if (config->verbose) {
    printf ("ERROR: Symbol name '%s' at line %u in file %s exceeded the character limit\n",
            symbol,
            config->line_number,
            config->file_name);
  }

  return TRUE;
}

bool_t SymbolUsedAsAMacro(char *symbol, macro_table_t *macro_list, syntax_check_config_t *config){

  if (NULL == FindMacro(macro_list, symbol)) {
    return FALSE;
  }

	if (config->verbose) {
    printf ("ERROR: Attempt to define a symbol '%s', but it was already defined as a macro at line %u in file %s\n",
            symbol,
            config->line_number,
            config->file_name);
  }

  return TRUE;
}

bool_t DirectiveDoesntExist(const char *directive, syntax_check_config_t *config) {
  int i = 0;
  while (i < NUM_OF_DIRECTIVES && strcmp(directive, reserved_directives[i])) {
    ++i;
  }
  
  if (i < NUM_OF_DIRECTIVES) {
    return FALSE;
  }
  
  if (config->verbose) {
    printf ("ERROR: The directive name '%s' in line %u in file %s doesn't exist\n",
            directive,
            config->line_number,
            config->file_name);
  }
  return TRUE;
}

bool_t CommaIsMissingInData(char *data, syntax_check_config_t *config){
   while (*data != '\0') {
	 if (*data == ' ' || *data == '\t' ){
		if (*(data-1) != ','){
			if (config->verbose){
                printf ("ERROR: a comma is missing between parameters at line %u in the file: %s", syntax_check_config.line_number, syntax_check_config.file_name);
            }
            return TRUE;
        }
     }
   }
   return FALSE;
}

bool_t RegisterNameDoesntExist(char *register_name, syntax_check_config_t *config){
    int i;
    for (i = 0; i < NUM_OF_REGISTERS; i++) {
        if (strcmp(register_name, register_names[i]) == 0) {
            return FALSE;
        }
    }
    if (config->verbose){
        printf ("ERROR: the register name at line %u in the file: %s doesn't exist", syntax_check_config.line_number, syntax_check_config.file_name);
    }
    return TRUE;
}



/*
*@brief Detects which type of addressing method fit to the operand, and returns invalid if neccessary.
*@param Operand - the operand
*@return The type of addressing method for the operand.
*/
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

  if ('*' == *ptr) {
    if (RegisterNameDoesntExist(ptr + 1, silent_syntax_cfg)) {
      return INVALID;
    }
    return INDIRECT_REGISTER;
  }
  if (!RegisterNameDoesntExist(ptr,silent_syntax_cfg)) {
    return DIRECT_REGISTER;
  }

  return DIRECT;
}

/*
 * @brief Checks if an instruction takes a source/destination operand.
 * @param instruction - The instruction to check.
 *        type - Operand type to check for (SOURCE_OPERAND or DESTINATION_OPERAND)
 *
 * @return TRUE if 'instruction' takes an operand whose type is 'type'.
 *         FALSE otherwise.
 */
static bool_t TakesOperand(instruction_t instruction, operand_type_t type) {
  return GetBitValue(instruction.addressing_info, type);
}

/*
 * @brief Checks if an addressing method is legal for an operand in a specific
*         instruction.
*
 * @param instruction - The instruction to check.
 *        type - Operand type to check for (SOURCE_OPERAND or DESTINATION_OPERAND)
 *        method - Addressing method to check.
 *
* @return TRUE If the instruction is legal, FALSE otherwise.
*/
static bool_t AddressingMethodIsLegal(instruction_t instruction,
                                      operand_type_t type,
                                      addressing_method_t method) {
  size_t bit_to_check = 2 + method + (type * 4);
  return GetBitValue(instruction.addressing_info, bit_to_check);
}
