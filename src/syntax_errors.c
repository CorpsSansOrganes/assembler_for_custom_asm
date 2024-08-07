#include <string.h> /* strchr */
#include "syntax_errors.h"
#include "utils.h"
#include "symbol_table.h"
#include "language_definitions.h"
#include  <ctype.h> 

/*
*@brief detects which type of addressing method fit to the operand, and returns invalid if neccessary.
*@param operand - the operand
*@return the type of addressing method for the operand
*/
static addressing_methods detect_addressing_method (char *operand);

static int SymbolCompare (void *str,void *key );


bool_t DetectExtraCharacters(const char *starting_from) {
  const char blank_chars[] = {' ', '\t'};
  char *ptr = (char *)starting_from;
  bool_t result = FALSE;

  while ('\0' != *ptr) {
    if (strchr(blank_chars, *ptr)) {
      result = TRUE;
      break;
    }
    ++ptr;
  }

  return result;
}

bool_t SymbolDefinedMoreThanOnce(char *symbol, symbol_table_t *table){
   int is_defined = (int) FindSymbol (table, symbol);
   if (is_defined != -1){
	return TRUE;
   }
   return FALSE;
}

bool_t SymbolWasntDefined(char *symbol, symbol_table_t *table){
   int is_defined = (int) FindSymbol (table, symbol);
   if (is_defined == -1){
	return FALSE;
   }
   return TRUE;
}

bool_t ColonSyntaxError(char *symbol){
    while (*(symbol+1) != '\0'){
       symbol++;
    }
    if (*symbol != ':'){
	return TRUE;
    }
    return FALSE;
}

int WrongNumberOfOperands(char *instruction, int num_of_operands){
    for (int i=0; i<NUM_OF_INSTRUCTIONS;i++){
        if (address_method_table[i].name == *instruction)
        {
            return num_of_operands - (address_method_table[i].is_there_destination_operand+address_method_table[i].is_there_source_operand);
        }
    }
}

bool_t IncorrectAddressingMethod(char *instruction, char *operand, operand_type_t type){
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

bool_t SymbolAlreadyDefinedAsEntry(char *symbol_name, symbol_table_t *table){
    symbol_t *extern_symbol = FindSymbol(table,symbol_name);
    return (GetSymbolType(symbol_name) == ENTRY);
}

bool_t SymbolAlreadyDefinedAsExtern(char *symbol_name, symbol_table_t *table){
    symbol_t *entry_symbol = FindSymbol(table,symbol_name);
    return (GetSymbolType(symbol_name) == EXTERN);
}

bool_t DoesInstructionExist(char *instruction){
   for (int i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
       if (strcmp(instruction, reserved_instructions[i]) == 0) {
           return TRUE;
       }
    }
}

bool_t SymbolIsIllegal(char *symbol) {

    for (int i = 0; i < strlen(symbol); i++)
     {
        if (!((*symbol >= 'A' && *symbol <= 'Z') || (*symbol >= 'a' && *symbol <= 'z') || (*symbol >= '0' && *symbol <= '9'))) {
            return TRUE;
        }
        symbol++;
    }

    return FALSE;
}

bool_t SymbolPrefixIllegal(char *symbol) {

    // Check if the first character is not an alphabetical character
    if (!((*symbol >= 'A' && *symbol <= 'Z') || (*symbol >= 'a' && *symbol <= 'z'))) {
        return TRUE;
    }

    return FALSE;
}

bool_t IsReservedName(char *name) {

    for (int i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
        if (strcmp(name, reserved_instructions[i]) == 0) {
            return TRUE;
        }
    }
    for (int i = 0; i < NUM_OF_DIRECTIVES; i++) {
        if (strcmp(name, reserved_directives[i]) == 0) {
            return TRUE;
        }

    }
    for (int i = 0; i < NUM_OF_REGISTERS; i++) {
        if (strcmp(name, register_names[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

bool_t RegisterNameDoesntExist(char *register_name){

    for (int i = 0; i < NUM_OF_REGISTERS; i++) {
        if (strcmp(register_name, register_names[i]) == 0) {
            return FALSE;
        }
    }

    return TRUE;
}

bool_t DirectiveDoesntExist(char *directive) {


    for (int i = 0; i < NUM_OF_DIRECTIVES; i++) {
        if (strcmp(directive, reserved_directives[i]) == 0) {
            return FALSE;
        }
    }

    return TRUE;
}

bool_t SymbolExceedCharacterLimit(char *symbol) {
    if (symbol == NULL) {
        return FALSE;
    }

    int length = 0;
    while (*symbol != '\0') {
        length++;
        if (length > 31) {
            return TRUE;
        }
        symbol++;
    }

    return FALSE;
}
bool_t SymbolUsedAsAMacro(char *symbol, macro_table_t *macro_list){
   if (NULL != Find (symbol, SymbolCompare, macro_list)){
	return TRUE;
   }  
   return FALSE;
}

bool_t CommaIsMissingInData(char *data){
   while (*data != '\0') {
	 if (*data == ' ' || *data == '\t' ){
		if (*(data-1) != ','){
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
        if (RegisterNameDoesntExist(*(operand+1))){
            return INVALID;
        }
        return INDIRECT_REGISTER;
    }
    if (!RegisterNameDoesntExist(*operand)){
        return DIRECT_REGISTER;
    }
    return DIRECT;
}

   
