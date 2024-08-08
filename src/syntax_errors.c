#include <string.h> /* strchr */
#include  <ctype.h> 
#include  <stdio.h>
#include "syntax_errors.h"
#include "utils.h"
#include "symbol_table.h"
#include "language_definitions.h"

/*
*@brief detects which type of addressing method fit to the operand, and returns invalid if neccessary.
*@param operand - the operand
*@return the type of addressing method for the operand
*/
static addressing_methods detect_addressing_method (char *operand);

static int SymbolCompare (void *str,void *key );

syntax_check_info_t syntax_check_default = {FALSE, 0, "default"};

bool_t DetectExtraCharacters(const char *starting_from, syntax_check_info_t syntax_check_info) {
  const char blank_chars[] = {' ', '\t'};
  char *ptr = (char *)starting_from;
  bool_t result = FALSE;

  while ('\0' != *ptr) {
    if (strchr(blank_chars, *ptr)) {
      if (syntax_check_info.verbose){
        printf ("Error: extrenuous characters detected at line %u in the file: %s", syntax_check_info.line_number, syntax_check_info.file_name);
      }
      result = TRUE;
      break;
    }
    ++ptr;
  }

  return result;
}

bool_t SymbolDefinedMoreThanOnce(char *symbol, symbol_table_t *table, syntax_check_info_t syntax_check_info){
   int is_defined = (int) FindSymbol (table, symbol);
   if (is_defined != -1){
	if (syntax_check_info.verbose){
        printf ("Error: attempted to define a symbol at line %u in the file: %s that already been defined", syntax_check_info.line_number, syntax_check_info.file_name);
    }
    return TRUE;
   }
   return FALSE;
}

bool_t SymbolWasntDefined(char *symbol, symbol_table_t *table, syntax_check_info_t syntax_check_info){
   int is_defined = (int) FindSymbol (table, symbol);
   if (is_defined == -1){
	return FALSE;
   }
    if (syntax_check_info.verbose){
        printf ("Error: attempted to call a symbol at line %u in the file: %s that wasn't defined", syntax_check_info.line_number, syntax_check_info.file_name);
    }
   return TRUE;
}

bool_t ColonSyntaxError(char *symbol, syntax_check_info_t syntax_check_info){
    while (*(symbol+1) != '\0'){
       symbol++;
    }
    if (*symbol != ':'){
        if (syntax_check_info.verbose){
        printf ("Error: symbol was used without colon or with space between the symbol and the colon at line %u in the file: %s", syntax_check_info.line_number, syntax_check_info.file_name);
        }
	return TRUE;
    }
    return FALSE;
}

int WrongNumberOfOperands(char *instruction, int num_of_operands, syntax_check_info_t syntax_check_info){
    int i;
    for (i=0; i<NUM_OF_INSTRUCTIONS;i++){
        if (*address_method_table[i].name == *instruction)
        {
            int required_operands = (address_method_table[i].is_there_destination_operand+address_method_table[i].is_there_source_operand);
            if (syntax_check_info.verbose){
                printf ("Error: required %d operands, but given %d. at line %u in the file: %s", required_operands, num_of_operands, syntax_check_info.line_number, syntax_check_info.file_name);
            }
            return num_of_operands-required_operands;        
        }
    }
}

bool_t IncorrectAddressingMethod(char *instruction, char *operand, operand_type_t type, syntax_check_info_t syntax_check_info){
    int i;
    int method = (int) detect_addressing_method (operand);
        for (i=0; i<NUM_OF_INSTRUCTIONS;i++){
            if (*address_method_table[i].name == *instruction){
                if (type == SOURCE_OPERAND){
                    if (!address_method_table[i].source_operand_methods[method]){
                        if(syntax_check_info.verbose){
                          printf ("Error: the addressing method of the operand %s at line %u in the file: %s is not compatible to the instruction", operand, syntax_check_info.line_number, syntax_check_info.file_name);
                        }
                        return TRUE;  
                    }
                    return FALSE;
                }
                else if (type == DESTINATION_OPERAND){
                    if (!address_method_table[i].destination_operand_methods[method]){
                        if(syntax_check_info.verbose){
                          printf ("Error: the addressing method of the operand %s at line %u in the file: %s is not compatible to the instruction", operand, syntax_check_info.line_number, syntax_check_info.file_name);
                        }
                        return TRUE;
                    }
                    return FALSE;
                }
            }
        }
    
}

bool_t SymbolAlreadyDefinedAsEntry(char *symbol_name, symbol_table_t *table, syntax_check_info_t syntax_check_info){
    symbol_t *extern_symbol = FindSymbol(table,symbol_name);
    if (GetSymbolType(extern_symbol) == ENTRY){
        if (syntax_check_info.verbose){
            printf ("ERROR: attempt to define an entry symbol at line %u in the file: %s as an extern", syntax_check_info.line_number, syntax_check_info.file_name);
        }
        return TRUE;
    }
    return FALSE;
}

bool_t SymbolAlreadyDefinedAsExtern(char *symbol_name, symbol_table_t *table, syntax_check_info_t syntax_check_info){
    symbol_t *entry_symbol = FindSymbol(table,symbol_name);
    if (GetSymbolType(entry_symbol) == EXTERN){
        if (syntax_check_info.verbose){
            printf ("ERROR: attempt to define an extern symbol at line %u in the file: %s as an entry", syntax_check_info.line_number, syntax_check_info.file_name);
        }
        return TRUE;
    }
    return FALSE;
}

bool_t InstructionDoesntExist(char *instruction, syntax_check_info_t syntax_check_info){
   int i;
   for (i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
       if (strcmp(instruction, reserved_instructions[i]) == 0) {
           return FALSE;
       }
    }
    if (syntax_check_info.verbose){
        printf ("ERROR: the instruction at line %u in the file: %s doesn't exist", syntax_check_info.line_number, syntax_check_info.file_name);
    }
    return TRUE;
}

bool_t SymbolIsIllegal(char *symbol, syntax_check_info_t syntax_check_info) {
    int i;
    for (i = 0; i < strlen(symbol); i++)
     {
        if (!((*symbol >= 'A' && *symbol <= 'Z') || (*symbol >= 'a' && *symbol <= 'z') || (*symbol >= '0' && *symbol <= '9'))) {
            if (syntax_check_info.verbose){
                printf ("ERROR: use of illegal characters in the symbol name at line %u in the file: %s", syntax_check_info.line_number, syntax_check_info.file_name);
            }
            return TRUE;
        }
        symbol++;
    }

    return FALSE;
}

bool_t SymbolPrefixIllegal(char *symbol, syntax_check_info_t syntax_check_info) {

    if (!((*symbol >= 'A' && *symbol <= 'Z') || (*symbol >= 'a' && *symbol <= 'z'))) {
        if (syntax_check_info.verbose){
            printf ("ERROR: illegal character in the prefix of a symbol at line %u in the file: %s", syntax_check_info.line_number, syntax_check_info.file_name);
        }
        return TRUE;
    }

    return FALSE;
}

bool_t IsReservedName(char *name, syntax_check_info_t syntax_check_info) {
    int i;
    for (i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
        if (strcmp(name, reserved_instructions[i]) == 0) {
            if (syntax_check_info.verbose){
                 printf ("ERROR: attempt to make use of a reserved instruction name at line %u in the file: %s ", syntax_check_info.line_number, syntax_check_info.file_name);
            }
            return TRUE;
        }
    }
    for (i = 0; i < NUM_OF_DIRECTIVES; i++) {
        if (strcmp(name, reserved_directives[i]) == 0) {
            if (syntax_check_info.verbose){
                printf ("ERROR: attempt to make use of a reserved directive name at line %u in the file: %s ", syntax_check_info.line_number, syntax_check_info.file_name);
            }
            return TRUE;
        }

    }
    for (i = 0; i < NUM_OF_REGISTERS; i++) {
        if (strcmp(name, register_names[i]) == 0) {
            if (syntax_check_info.verbose){
                printf ("ERROR: attempt to make use of a reserved register name at line %u in the file: %s ", syntax_check_info.line_number, syntax_check_info.file_name);
            }
            return TRUE;
        }
    }
    return FALSE;
}

bool_t RegisterNameDoesntExist(char *register_name, syntax_check_info_t syntax_check_info){
    int i;
    for (i = 0; i < NUM_OF_REGISTERS; i++) {
        if (strcmp(register_name, register_names[i]) == 0) {
            return FALSE;
        }
    }
    if (syntax_check_info.verbose){
        printf ("ERROR: the register name at line %u in the file: %s doesn't exist", syntax_check_info.line_number, syntax_check_info.file_name);
    }
    return TRUE;
}

bool_t DirectiveDoesntExist(char *directive, syntax_check_info_t syntax_check_info) {
    int i;
    for (i = 0; i < NUM_OF_DIRECTIVES; i++) {
        if (strcmp(directive, reserved_directives[i]) == 0) {
            return FALSE;
        }
    }
    if (syntax_check_info.verbose){
        printf ("ERROR: the directive name at line %u in the file: %s doesn't exist", syntax_check_info.line_number, syntax_check_info.file_name);
    }
    return TRUE;
}
bool_t DirectiveIsUpperCase(char *directive, syntax_check_info_t syntax_check_info){
    if (DirectiveDoesntExist(directive, syntax_check_default) && !DirectiveDoesntExist(tolower(directive), syntax_check_default))
    {
        if (syntax_check_info.verbose){
            printf ("ERROR: directive was written in uppercase letters instead of lowercase letters at line: %u in the file: %s doesn't exist", syntax_check_info.line_number, syntax_check_info.file_name);
        }
        return TRUE;
    }
    return FALSE;
}

bool_t SymbolExceedCharacterLimit(char *symbol, syntax_check_info_t syntax_check_info) {
    if (symbol == NULL) {
        return FALSE;
    }

    int length = 0;
    while (*symbol != '\0') {
        length++;
        if (length > 31) {
            if (syntax_check_info.verbose){
                printf ("ERROR: the name of the symbol at line %u in the file: %s exceeded the character limit", syntax_check_info.line_number, syntax_check_info.file_name);
            }
            return TRUE;
        }
        symbol++;
    }

    return FALSE;
}
bool_t SymbolUsedAsAMacro(char *symbol, macro_table_t *macro_list, syntax_check_info_t syntax_check_info){
   if (NULL != Find (symbol, SymbolCompare, macro_list)){
	if (syntax_check_info.verbose){
        printf ("ERROR: attempt to define a symbol name that is already define as a macro at line %u in the file: %s", syntax_check_info.line_number, syntax_check_info.file_name);
    }
    return TRUE;
   }  
   return FALSE;
}

bool_t CommaIsMissingInData(char *data, syntax_check_info_t syntax_check_info){
   while (*data != '\0') {
	 if (*data == ' ' || *data == '\t' ){
		if (*(data-1) != ','){
			if (syntax_check_info.verbose){
                printf ("ERROR: a comma is missing between parameters at line %u in the file: %s", syntax_check_info.line_number, syntax_check_info.file_name);
            }
            return TRUE;
        }
     }
   }
   return FALSE;
}

static int SymbolCompare (void *str,void *key ){
   return (0 == strcmp ((char *) str, (char *) key ));  
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
                
    }
        return DIRECT;
    if (*operand == '*'){
        if (RegisterNameDoesntExist(*(operand+1),syntax_check_default)){
            return INVALID;
        }
        return INDIRECT_REGISTER;
    }
    if (!RegisterNameDoesntExist(*operand,syntax_check_default)){
        return DIRECT_REGISTER;
    }
    return DIRECT;
}

   
