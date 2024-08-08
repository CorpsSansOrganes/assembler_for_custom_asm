#include <string.h> /* strchr */
#include  <ctype.h> 
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


bool_t DetectExtraCharacters(const char *starting_from, error_check_info_t error_check_info) {
  const char blank_chars[] = {' ', '\t'};
  char *ptr = (char *)starting_from;
  bool_t result = FALSE;

  while ('\0' != *ptr) {
    if (strchr(blank_chars, *ptr)) {
      if (error_check_info.verbose){
        printf ("Error: extrenuous characters detected at line %u in the file: %s", error_check_info.line_number, error_check_info.file_name);
      }
      result = TRUE;
      break;
    }
    ++ptr;
  }

  return result;
}

bool_t SymbolDefinedMoreThanOnce(char *symbol, symbol_table_t *table, error_check_info_t error_check_info){
   int is_defined = (int) FindSymbol (table, symbol);
   if (is_defined != -1){
	if (error_check_info.verbose){
        printf ("Error: attempted to define a symbol at line %u in the file: %s that already been defined", error_check_info.line_number, error_check_info.file_name);
    }
    return TRUE;
   }
   return FALSE;
}

bool_t SymbolWasntDefined(char *symbol, symbol_table_t *table, error_check_info_t error_check_info){
   int is_defined = (int) FindSymbol (table, symbol);
   if (is_defined == -1){
	return FALSE;
   }
    if (error_check_info.verbose){
        printf ("Error: attempted to call a symbol at line %u in the file: %s that wasn't defined", error_check_info.line_number, error_check_info.file_name);
    }
   return TRUE;
}

bool_t ColonSyntaxError(char *symbol, error_check_info_t error_check_info){
    while (*(symbol+1) != '\0'){
       symbol++;
    }
    if (*symbol != ':'){
        if (error_check_info.verbose){
        printf ("Error: symbol was used without colon or with space between the symbol and the colon at line %u in the file: %s", error_check_info.line_number, error_check_info.file_name);
        }
	return TRUE;
    }
    return FALSE;
}

int WrongNumberOfOperands(char *instruction, int num_of_operands, error_check_info_t error_check_info){
    for (int i=0; i<NUM_OF_INSTRUCTIONS;i++){
        if (address_method_table[i].name == *instruction)
        {
            int required_operands = (address_method_table[i].is_there_destination_operand+address_method_table[i].is_there_source_operand);
            if (error_check_info.verbose){
                printf ("Error: required %d operands, but given %d. at line %u in the file: %s", required_operands, num_of_operands, error_check_info.line_number, error_check_info.file_name);
            }
            return num_of_operands-required_operands;        
        }
    }
}

/*to add print error!*/
bool_t IncorrectAddressingMethod(char *instruction, char *operand, operand_type_t type, error_check_info_t error_check_info){
    int method = (int) detect_addressing_method (operand);
        for (int i=0; i<NUM_OF_INSTRUCTIONS;i++){
            if (address_method_table[i].name == *instruction){
                if (type == SOURCE_OPERAND){
                    return (!address_method_table[i].source_operand_methods[method]);
                }
                else if (type == DESTINATION_OPERAND){
                    return (!address_method_table[i].destination_operand_methods[method]);
                }
            }
        }
    
}

/*to add print error!*/
bool_t SymbolAlreadyDefinedAsEntry(char *symbol_name, symbol_table_t *table, error_check_info_t error_check_info){
    symbol_t *extern_symbol = FindSymbol(table,symbol_name);
    return (GetSymbolType(symbol_name) == ENTRY);
}

/*to add print error!*/

bool_t SymbolAlreadyDefinedAsExtern(char *symbol_name, symbol_table_t *table, error_check_info_t error_check_info){
    symbol_t *entry_symbol = FindSymbol(table,symbol_name);
    return (GetSymbolType(symbol_name) == EXTERN);
}

bool_t InstructionDoesntExist(char *instruction, error_check_info_t error_check_info){
   for (int i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
       if (strcmp(instruction, reserved_instructions[i]) == 0) {
           return FALSE;
       }
    }
    if (error_check_info.verbose){
        printf ("ERROR: the instruction at line %u in the file: %s doesn't exist", error_check_info.line_number, error_check_info.file_name);
    }
    return TRUE;
}

bool_t SymbolIsIllegal(char *symbol, error_check_info_t error_check_info) {

    for (int i = 0; i < strlen(symbol); i++)
     {
        if (!((*symbol >= 'A' && *symbol <= 'Z') || (*symbol >= 'a' && *symbol <= 'z') || (*symbol >= '0' && *symbol <= '9'))) {
            if (error_check_info.verbose){
                printf ("ERROR: use of illegal characters in the symbol name at line %u in the file: %s", error_check_info.line_number, error_check_info.file_name);
            }
            return TRUE;
        }
        symbol++;
    }

    return FALSE;
}

bool_t SymbolPrefixIllegal(char *symbol, error_check_info_t error_check_info) {

    // Check if the first character is not an alphabetical character
    if (!((*symbol >= 'A' && *symbol <= 'Z') || (*symbol >= 'a' && *symbol <= 'z'))) {
        if (error_check_info.verbose){
            printf ("ERROR: illegal character in the prefix of a symbol at line %u in the file: %s", error_check_info.line_number, error_check_info.file_name);
        }
        return TRUE;
    }

    return FALSE;
}

bool_t IsReservedName(char *name, error_check_info_t error_check_info) {

    for (int i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
        if (strcmp(name, reserved_instructions[i]) == 0) {
            if (error_check_info.verbose){
                 printf ("ERROR: attempt to make use of a reserved instruction name at line %u in the file: %s ", error_check_info.line_number, error_check_info.file_name);
            }
            return TRUE;
        }
    }
    for (int i = 0; i < NUM_OF_DIRECTIVES; i++) {
        if (strcmp(name, reserved_directives[i]) == 0) {
            if (error_check_info.verbose){
                printf ("ERROR: attempt to make use of a reserved directive name at line %u in the file: %s ", error_check_info.line_number, error_check_info.file_name);
            }
            return TRUE;
        }

    }
    for (int i = 0; i < NUM_OF_REGISTERS; i++) {
        if (strcmp(name, register_names[i]) == 0) {
            if (error_check_info.verbose){
                printf ("ERROR: attempt to make use of a reserved register name at line %u in the file: %s ", error_check_info.line_number, error_check_info.file_name);
            }
            return TRUE;
        }
    }
    return FALSE;
}

bool_t RegisterNameDoesntExist(char *register_name, error_check_info_t error_check_info){

    for (int i = 0; i < NUM_OF_REGISTERS; i++) {
        if (strcmp(register_name, register_names[i]) == 0) {
            return FALSE;
        }
    }
    if (error_check_info.verbose){
        printf ("ERROR: the register name at line %u in the file: %s doesn't exist", error_check_info.line_number, error_check_info.file_name);
    }
    return TRUE;
}

bool_t DirectiveDoesntExist(char *directive, error_check_info_t error_check_info) {


    for (int i = 0; i < NUM_OF_DIRECTIVES; i++) {
        if (strcmp(directive, reserved_directives[i]) == 0) {
            return FALSE;
        }
    }
    if (error_check_info.verbose){
        printf ("ERROR: the directive name at line %u in the file: %s doesn't exist", error_check_info.line_number, error_check_info.file_name);
    }
    return TRUE;
}
bool_t DirectiveIsUpperCase(char *directive, error_check_info_t error_check_info){
    if (DirectiveDoesntExist(directive, verbose) && !DirectiveDoesntExist(tolower(directive), verbose))
    {
        if (error_check_info.verbose){
            printf ("ERROR: directive was written in uppercase letters instead of lowercase letters at line: %u in the file: %s doesn't exist", error_check_info.line_number, error_check_info.file_name);
        }
        return TRUE;
    }
    return FALSE;
}

bool_t SymbolExceedCharacterLimit(char *symbol, error_check_info_t error_check_info) {
    if (symbol == NULL) {
        return FALSE;
    }

    int length = 0;
    while (*symbol != '\0') {
        length++;
        if (length > 31) {
            if (error_check_info.verbose){
                printf ("ERROR: the name of the symbol at line %u in the file: %s exceeded the character limit", error_check_info.line_number, error_check_info.file_name);
            }
            return TRUE;
        }
        symbol++;
    }

    return FALSE;
}
bool_t SymbolUsedAsAMacro(char *symbol, macro_table_t *macro_list, error_check_info_t error_check_info){
   if (NULL != Find (symbol, SymbolCompare, macro_list)){
	if (error_check_info.verbose){
        printf ("ERROR: attempt to define a symbol name that is already define as a macro at line %u in the file: %s", error_check_info.line_number, error_check_info.file_name);
    }
    return TRUE;
   }  
   return FALSE;
}

bool_t CommaIsMissingInData(char *data, error_check_info_t error_check_info){
   while (*data != '\0') {
	 if (*data == ' ' || *data == '\t' ){
		if (*(data-1) != ','){
			if (error_check_info.verbose){
                printf ("ERROR: a comma is missing between parameters at line %u in the file: %s", error_check_info.line_number, error_check_info.file_name);
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
    if (*operand == '#'){
        operand++;
        if (*operand == '-' || *operand == '+'){
            operand++;
        }
        if (strlen(operand)==0){
            return INVALID;
        }
        for (int i = 0; i < strlen(operand); i++)
        {
           if ( ! isdigit (*operand+i))
           {
            return INVALID;
           }
        }
                
    }
        return DIRECT;
    if (*operand == '*'){
        if (RegisterNameDoesntExist(*(operand+1),FALSE)){
            return INVALID;
        }
        return INDIRECT_REGISTER;
    }
    if (!RegisterNameDoesntExist(*operand,FALSE)){
        return DIRECT_REGISTER;
    }
    return DIRECT;
}

   
