/* syntax_errors.c
 *
 * This module contains all error checking functionalities. 
 * It gives the option to decide whether to print error message.
 */

#include <string.h> /* strchr */
#include <ctype.h> /* isalpha */
#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free */
#include <limits.h> /*max_int*/
#include "syntax_errors.h"
#include "utils.h"
#include "symbol_table.h"
#include "language_definitions.h"
#include "string_utils.h"

#define BASE_10 (10)
#define delimiters ", \n\t\r"

static bool_t TakesOperand(instruction_t instruction, operand_type_t type);
static bool_t AddressingMethodIsLegal(instruction_t instruction,
                                      operand_type_t type,
                                      addressing_method_t method);
static bool_t IsDataEntryValid(const char *from,
                               const char *to);

static bool_t SymbolPrefixIllegal(const char *symbol,
                                  syntax_check_config_t *config);

static bool_t SymbolExceedCharacterLimit(const char *symbol,
                                  syntax_check_config_t *config);

static bool_t DataParametersTooBig(const char *params,
                                   syntax_check_config_t *config);

static bool_t IsDataParameterTooBig(const char *parameter);
                                  
static bool_t StringIsNotPrintable (const char *str,
                                    syntax_check_config_t *config);

/* This config is used for calling other syntax checks internally silently */
syntax_check_config_t silent_syntax_cfg = {NULL, 0, FALSE};

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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Extraneous characters detected ('%s')\n\n",
            config->file_name,
            config->line_number,
            starting_from);
  }
  return TRUE;
}

bool_t IsReservedName(const char *name, syntax_check_config_t *config) {
  if (FALSE == InstructionDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Attempt to make use of a reserved instruction name '%s' \n\n",
              config->file_name,
              config->line_number,
              name);
    }
    return TRUE;
  }

  if (INVALID_DIRECTIVE != IdentifyDirective(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Attempt to make use of a reserved directive name '%s' \n\n",
              config->file_name,
              config->line_number,
              name);
    }
    return TRUE;
  }

  if (FALSE == RegisterNameDoesntExist(name, &silent_syntax_cfg)) {
    if (config->verbose) {
      printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Attempt to make use of a reserved register name '%s' \n\n",
              config->file_name,
              config->line_number, 
              name);
    }
    return TRUE;
  }

  return FALSE;
}

bool_t MacroDefinedMoreThanOnce(const char *macro_name,
                                macro_table_t *table,
                                syntax_check_config_t *config) {

  if (NULL == FindMacro(table, macro_name)) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Attempt to redefine macro with the name '%s'.\n\n",
            config->file_name,
            config->line_number, 
            macro_name);
  }

  return TRUE;
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Unknown instruction '%s' \n\n",
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n For instruction '%s' expected %d operands, but given %d \n\n",
            config->file_name,
            config->line_number,
            instruction,
            required_operands,
            num_of_operands);
  }
  return TRUE;
}

bool_t IsOperandInvalid(const operand_t *operand,
                        syntax_check_config_t *config) {
  if (INVALID != operand->addressing_method) {
    return FALSE;
  }

  if (config->verbose) {
    printf(BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n invalid operand '%s'.",
           config->file_name,
           config->line_number,
           operand->name
      );
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
    const char *method_str;

    switch (method) {
      case IMMEDIATE:
        method_str = "immediate";
        break;
      case DIRECT:
        method_str = "direct";
        break;
      case INDIRECT_REGISTER:
        method_str = "indirect register";
        break;
      case DIRECT_REGISTER:
        method_str = "direct register";
        break;
      case INVALID:
        method_str = "invalid";
        break;
    }

    printf(BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n ",
            config->file_name,
            config->line_number
      );

    printf("operand '%s' can't be used as %s operand in the instruction '%s' ",
            operand->name, 
            (operand->type ? "source" : "target"),
            instruction
      );

    printf("(%s addressing isn't supported for %s operands)\n\n",
            method_str,
            (operand->type ? "source" : "target")
      );
  }
  return TRUE;
}

bool_t NoSpaceAfterColon(const char *line,
                         syntax_check_config_t *config) {
  char *colon = strchr(line, ':');
  if (NULL == colon) {
    return FALSE;
  }
  if (' ' == *(colon + 1)) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n No space after symbol name definition '%s'.\n\n",
            config->file_name,
            config->line_number,
            line);
  }

  return TRUE;
}

bool_t SymbolDefinedMoreThanOnce(const char *symbol,
                                 symbol_table_t *table,
                                 syntax_check_config_t *config) {

  if (TRUE == SymbolWasntDefined(symbol, table, &silent_syntax_cfg)) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n The symbol '%s' was already defined.\n\n",
            config->file_name,
            config->line_number,
            symbol);
  }
  return TRUE;
}

bool_t SymbolWasntDefined(const char *symbol,
                          symbol_table_t *table,
                          syntax_check_config_t *config){
  if (NULL != FindSymbol (table, symbol)) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Attempted to call a symbol that wasn't defined \n\n",
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
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Attempt to define '%s' as entry, but it was already defined as extern\n\n",
            config->file_name,
            config->line_number,
            symbol_name);
  }
  return TRUE;
}

bool_t SymbolNameIsIllegal(const char *symbol, syntax_check_config_t *config) {
  char *ptr = (char *)symbol;
  bool_t result = FALSE;

  if (SymbolPrefixIllegal(ptr, config)) {
    result = TRUE;
  }

  if (SymbolExceedCharacterLimit(symbol, config)) {
    result = TRUE;
  }

  while ('\0' != *ptr && (isalpha(*ptr) || isdigit(*ptr))) {
    ++ptr;
  }

  if ('\0' == *ptr) {
    return result;
  } 
  else {
    result = TRUE;
  }


  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Use of illegal characters in the symbol name '%s'\n\n",
            config->file_name,
            config->line_number,
            symbol);
  }

  return result;
}


bool_t SymbolUsedAsAMacro(const char *symbol,
                          macro_table_t *macros,
                          syntax_check_config_t *config) {

  if (NULL == FindMacro(macros, symbol)) {
    return FALSE;
  }

	if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Attempt to define a symbol '%s', but it was already defined as a macro \n\n",
            config->file_name,
            config->line_number,
            symbol);
  }

  return TRUE;
}

bool_t NoDefinitionForSymbol(const char *after_symbol, syntax_check_config_t *config) {
  if (NULL == after_symbol) {
    return TRUE;
  }

  if (TRUE == DetectExtraCharacters(after_symbol, &silent_syntax_cfg)) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n No definition after symbol label \n\n",
            config->file_name,
            config->line_number);
  }
  return TRUE;
}

bool_t ImmediateOperandTooBig (operand_t *operand,
                               syntax_check_config_t *config) {
  long value = 0;

  value = strtol(operand->name + 1, NULL, BASE_10); /* skip the # */
  if (value > MAX_IMMEDIATE_OPERAND || value < MIN_IMMEDIATE_OPERAND) {
        if (config->verbose) {
            printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n immediate operand '%s' exceeds limit (max: %d, min :%d) \n\n",
            config->file_name,
            config->line_number,
            operand->name,
            MAX_IMMEDIATE_OPERAND,
            MIN_IMMEDIATE_OPERAND);
        }
        return TRUE;
  }
  return FALSE;
}

static bool_t DataParametersTooBig(const char *params,
                                   syntax_check_config_t *config) {
    char *duplicate_line = StrDup(params); 
    char *ptr = strtok(duplicate_line, " ,/t/n/r");

    while (ptr != NULL) {
    if (TRUE == IsDataParameterTooBig(ptr)) {
      if (config->verbose) {
        printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n data parameter '%s' is exceeds limit \n\n",
        config->file_name,
          config->line_number,
          ptr);
      }
      free(duplicate_line);
      return TRUE;
    }

    ptr = strtok(NULL, " ,/t/n/r");
  }

  free(duplicate_line);
  return FALSE;
}


directive_t IdentifyDirective(const char *directive, syntax_check_config_t *config) {
  directive_t d = 0;
  while (d < NUM_OF_DIRECTIVES && strcmp(directive, reserved_directives[d])) {
    ++d;
  }
  
  if (d != INVALID_DIRECTIVE) {
    return d;
  }
  
  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n The directive name '%s' doesn't exist \n\n",
            config->file_name,
            config->line_number,
            directive);
  }
  return INVALID_DIRECTIVE;
}

bool_t IsIllegalDataParameter(const char *data, syntax_check_config_t *config) {
  char *start = (char *)data;
  char *end = strstr(start, ",");
  bool_t result = FALSE;

  if (DataParametersTooBig(data, config)) {
    result = TRUE;
  }

  while (NULL != end && IsDataEntryValid(start, end)) {
    start = end + 1;
    end = strstr(start, ",");
  }

  if (NULL == end && IsDataEntryValid(start, end)) {
    return result;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n .data definition '%s' is invalid \n\n",
            config->file_name,
            config->line_number,
            data);
  }

  return TRUE;
}

bool_t IsIllegalString(const char *str, syntax_check_config_t *config) {
  const char *start = str;
  bool_t res = FALSE;

  if (StringIsNotPrintable(str, config)) {
    res = TRUE;
  }

  if ('\"' == *str) {
    str = EndOfString(str) - 1;
    while (IsBlank(*str)) {
      --str;
    }
    
    if ('\"' == *str) {
      return res;
    }
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n .string definition '%s' is invalid \n\n",
            config->file_name,
            config->line_number,
            start);
  }

  return TRUE;
}

bool_t AreCommasMisplaced(const char *param, syntax_check_config_t *config) {
  const char *ptr = param;
  bool_t comma_is_legal = FALSE;
  bool_t last_char_legal = TRUE;
  bool_t char_is_legal = TRUE;
  bool_t error = FALSE;

  while ('\0' != *ptr) {
    /* Handle whitespaces - if they come after a name, comma is expected */
    if (IsBlank(*ptr)) {
      if (TRUE == char_is_legal && TRUE == comma_is_legal) {
        /* If we have a blank space after a parameter, only comma is legal */
        char_is_legal = FALSE;
      }
    }

    /*Handle commas - after comma only chars are legal, and cannot finish woth comma*/
    else if (',' == *ptr) {
      if (FALSE == comma_is_legal) {
        error = TRUE;
        break;
      }
      comma_is_legal = FALSE;
      char_is_legal = TRUE;
      last_char_legal = FALSE;
    }  
    /*after char everything legal, and its legal to finish here*/
    else {
      if (FALSE == char_is_legal) {
        error = TRUE;
        break;
      }
      char_is_legal = TRUE;
      comma_is_legal = TRUE;
      last_char_legal = TRUE;
    }
    ptr++;
  }

  if (FALSE == error && TRUE == last_char_legal) {
    return FALSE;
  }

  if (config->verbose) {
      printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Commas placed in parameters '%s' are misplaced \n\n",
              config->file_name,
              config->line_number,
              param);
  }

  return TRUE;
}


static bool_t StringIsNotPrintable (const char *str,
                                    syntax_check_config_t *config) {
  const char *ptr = str;

  while ('\0' != *ptr && isprint(*ptr)) {
    ++ptr;
  }

  /* Non-printable char detected */
  if ('\0' != *ptr) {
    if (config->verbose) {
      printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n string '%s' is unprintable \n\n",
              config->file_name,
              config->line_number,
              str);
    }
    
    return TRUE;
  }

  return FALSE;
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
      printf(BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Register name '%s' doesn't exist \n\n",
            config->file_name,
            config->line_number,
            register_name);
  }
  return TRUE;
}

syntax_check_config_t CreateSyntaxCheckConfig(const char *file_name,
                                              unsigned int line_number,                                          
                                              bool_t verbose) {
  syntax_check_config_t info;
  info.file_name = file_name;
  info.line_number = line_number;
  info.verbose = verbose;

  return info;
}

addressing_method_t DetectAddressingMethod(const char *operand_name) {
  char *ptr = (char *)operand_name;

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

  else if (SymbolNameIsIllegal(ptr, &silent_syntax_cfg)) {
    return INVALID;
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

  /* Skip whitespaces */
  while (IsBlank(*from)) from++;

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
  
  while (IsBlank(*from)) from++;

  if (digits_occurred && 
      ((from == to) || ('\0' == *from))) {
    return TRUE;
  }

  return FALSE;
}

/*
 * @brief Tell if a symbol begins with illegal characters 
 *        (must begin with alphabetical characters only)
 *
 * @param symbol - The symbol name.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol is illegal, or FALSE otherwise.
 */

static bool_t SymbolPrefixIllegal(const char *symbol, syntax_check_config_t *config) {
  if (isalpha(*symbol)) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Symbols '%s' doesn't start with an alphabetical character \n\n",
            config->file_name,
            config->line_number,
            symbol);
  }

  return TRUE;
}

/*
 * @brief Check if the length of the symbol name exceeded the character limit (31).
 *
 * @param symbol - The symbol.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol exceeded the character limit, or FALSE otherwise.
 */

static bool_t SymbolExceedCharacterLimit(const char *symbol,
                                  syntax_check_config_t *config) {
  unsigned int length = 0;
  const char *ptr = symbol;

  while ('\0' != *ptr && length <= SYMBOL_CHARACTER_LIMIT) {
    length++;
    ptr++;
  }

  if (length <= SYMBOL_CHARACTER_LIMIT) {
    return FALSE;
  }

  if (config->verbose) {
    printf (BOLD_RED "ERROR " COLOR_RESET "(file %s, line %u):\n Symbol name '%s' exceeded the character limit \n\n",
            config->file_name,
            config->line_number,
            symbol);
  }

  return TRUE;
}

static bool_t IsDataParameterTooBig(const char *parameter) {
  long value = 0;

  value = strtol(parameter, NULL, BASE_10);
  if (value < MIN_DATA_PARAMETER || value > MAX_DATA_PARAMETER) {
    return TRUE;
  }

  return FALSE;
}

