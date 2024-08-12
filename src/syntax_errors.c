#include <string.h> /* strchr */
#include <ctype.h> /* isalpha */
#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free */
#include "syntax_errors.h"
#include "utils.h"
#include "symbol_table.h"
#include "language_definitions.h"
#include "string_utils.h"

static addressing_method_t DetectAddressingMethod(const char *operand);
static bool_t TakesOperand(instruction_t instruction, operand_type_t type);
static bool_t AddressingMethodIsLegal(instruction_t instruction,
                                      operand_type_t type,
                                      addressing_method_t method);
static bool_t IsDataEntryValid(const char *from,
                               const char *to);

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

/*
* @brief Detects what kind of addressing method a given operand adherse to.
*        For example:
*        #3 - Immediate addresing.
*        r1 - Direct register addressing.
*        *r3 - Indirect register addresing.
*
* @param operand - the operand name, e.g. "#3".
*
* @return The type of addressing method for the operand.
*         In particular, if the 
*/

addressing_method_t DetectAddressingMethod(const char *operand_name) {
  char *ptr = (char *)StripLeadingWhitespaces(operand_name);

  if ('#' == *ptr) {
    bool_t digit_occurred = FALSE;
    ptr++;
    if (('-' == *ptr) || ('+' ==  *ptr)) {
      ptr++;
    }

    if (isdigit(*ptr)) {
      digit_occurred = TRUE;
      ++ptr;
    }

    while (isdigit(*ptr)) {
      ++ptr;
    }

    if (('\0' != *ptr) || (FALSE == digit_occurred)) {
      return INVALID;
    }

    return IMMEDIATE;                
  }

  else if ('*' == *ptr) {
    ++ptr;
    if (RegisterNameDoesntExist(ptr, &silent_syntax_cfg)) {
      return INVALID;
    }
    return INDIRECT_REGISTER;
  }

  else if (!RegisterNameDoesntExist(ptr, &silent_syntax_cfg)) {
    return DIRECT_REGISTER;
  }

  return DIRECT;
}

bool_t DetectExtraCharacters(const char *starting_from,
                             syntax_check_config_t *config) {
  char *ptr = (char *)starting_from;

  while (IsBlank(*ptr)) {
    ++ptr;
  }

  if ('\0' == *ptr) {
    return FALSE;
  }

  else if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Extraneous characters detected ('%s')\n",
            config->file_name,
            config->line_number,
            starting_from);
  }
  return TRUE;
}

bool_t IsReservedName(const char *name, syntax_check_config_t *config) {
  if (FALSE == InstructionDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Attempt to make use of a reserved instruction name '%s' \n",
              config->file_name,
              config->line_number,
              name);
    }
    return TRUE;
  }

  if (FALSE == DirectiveDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Attempt to make use of a reserved directive name '%s' \n",
              config->file_name,
              config->line_number,
              name);
    }
    return TRUE;
  }

  if (FALSE == RegisterNameDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Attempt to make use of a reserved register name '%s' \n",
              config->file_name,
              config->line_number, 
              name);
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Unknown instruction '%s' \n",
            config->file_name,
            config->line_number,
            instruction);
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): For instruction '%s' expected %d operands, but given %d \n",
            config->file_name,
            config->line_number,
            instruction,
            required_operands,
            num_of_operands);
  }
  return TRUE;
}

bool_t IncorrectAddressingMethod(const char *instruction,
                                 const operand_t *operand,
                                 syntax_check_config_t *config) {
  int i = 0;
  addressing_method_t method = operand->addressing_method;
  operand_type_t type = operand->type;

  while (0 != strcmp(reserved_instructions[i].name, instruction)) {
    ++i;
  }

  if (TRUE == AddressingMethodIsLegal(reserved_instructions[i], type, method)) {
    return FALSE;
  }

  if (config->verbose) {
    printf(BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Addressing method of the operand '%s' is illegal for the instruction '%s' \n",
           config->file_name,
           config->line_number,
           operand->name, 
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Attempted to define a symbol that was already defined \n",
            config->file_name,
            config->line_number);
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Attempted to call a symbol that wasn't defined \n",
            config->file_name,
            config->line_number);
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Attempt to define an entry symbol as extern \n",
            config->file_name,
            config->line_number);
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Attempt to define an extern symbol as entry \n",
            config->file_name,
            config->line_number);
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Use of illegal characters in the symbol name '%s'\n",
            config->file_name,
            config->line_number,
            symbol);
  }
  return TRUE;
}

bool_t SymbolPrefixIllegal(const char *symbol, syntax_check_config_t *config) {
  if (isalpha(*symbol)) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Symbols '%s' doesn't start with an alphabetical character \n",
            config->file_name,
            config->line_number,
            symbol);
  }

  return TRUE;
}


bool_t SymbolExceedCharacterLimit(const char *symbol,
                                  syntax_check_config_t *config) {
  unsigned int length = 0;

  while ('\0' != *symbol && length <= SYMBOL_CHARACTER_LIMIT) {
    length++;
    symbol++;
  }

  if (length <= SYMBOL_CHARACTER_LIMIT) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Symbol name '%s' exceeded the character limit \n",
            config->file_name,
            config->line_number,
            symbol);
  }

  return TRUE;
}

bool_t SymbolUsedAsAMacro(char *symbol,
                          macro_table_t *macro_list,
                          syntax_check_config_t *config) {

  if (NULL == FindMacro(macro_list, symbol)) {
    return FALSE;
  }

	if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Attempt to define a symbol '%s', but it was already defined as a macro \n",
            config->file_name,
            config->line_number,
            symbol);
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): The directive name '%s' doesn't exist \n",
            config->file_name,
            config->line_number,
            directive);
  }
  return TRUE;
}

bool_t IsIllegalDataParameter(const char *data, syntax_check_config_t *config) {
  char *start = (char *)data;
  char *end = strstr(start, ",");

  while (NULL != end && IsDataEntryValid(start, end)) {
    start = end + 1;
    end = strstr(start, ",");
  }

  if (NULL == end && IsDataEntryValid(start, end)) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): .data definition '%s' is invalid \n",
            config->file_name,
            config->line_number,
            data);
  }

  return TRUE;
}

bool_t IsIllegalString(const char *str, syntax_check_config_t *config) {
  const char *start = str;

  if ('\"' == *str) {
    str = EndOfString(str);
    while (IsBlank(*str)) {
      --str;
    }
    
    if ('\"' == *str) {
      return FALSE;
    }
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): .string definition '%s' is invalid \n",
            config->file_name,
            config->line_number,
            start);
  }

  return TRUE;
}

bool_t RegisterNameDoesntExist(const char *register_name, syntax_check_config_t *config){
  int i = 0;
  while (i < NUM_OF_REGISTERS && strcmp(register_name, register_names[i])) {
    ++i;
  }
  
  if (i < NUM_OF_REGISTERS) {
    return FALSE;
  }
  
  if (config->verbose) {
      printf(BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u): Register name '%s' doesn't exist \n",
            config->file_name,
            config->line_number,
            register_name);
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
    if (('-' == *ptr) || ('+' ==  *ptr)) {
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
    if (RegisterNameDoesntExist(ptr + 1, &silent_syntax_cfg)) {
      return INVALID;
    }
    return INDIRECT_REGISTER;
  }
  if (!RegisterNameDoesntExist(ptr, &silent_syntax_cfg)) {
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

/*
 * @brief Checks if a given .data parameter represents a valid integer number.
 * 
 * This function verifies whether the substring defined by the pointers `from` and `to`
 * represents a valid integer number. The function allows an optional leading sign 
 * ('+' or '-'), as well as leading & trailing whitespaces.
 * 
 * If `to` is `NULL`, the function checks the substring starting from `from` and ending at 
 * the null terminator (`'\0'`).
 * If `to` is not `NULL`, it checks the substring [from, to)
 * 
 * @param from - A pointer to the start of the substring to be checked.
 * @param to - A pointer to the end of the substring to be checked, or `NULL` if the function 
 *           should check up to the null terminator.
 *
 * @return TRUE if the substring represents a valid integer number, FALSE otherwise.
 */

static bool_t IsDataEntryValid(const char *from,
                               const char *to) {
  bool_t digits_occurred = FALSE;

  from = StripLeadingWhitespaces(from);
  if (('-' == *from) || ('+' == *from)) {
    ++from;
  }

  if (isdigit(*from)) {
    digits_occurred = TRUE;
    ++from;
  }

  while (isdigit(*from)) {
    ++from;
  }
  
  from = (char *)StripLeadingWhitespaces(from);

  if (digits_occurred && 
      ((from == to) || ('\0' == *from))) {
    return TRUE;
  }

  return FALSE;
}
