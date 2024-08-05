#include "syntax_errors.h"
#include "utils.h"
#include "symbol_table.h"
#include "language_definitions.h"

typedef bool bool_t;

bool_t DetectExtraCharacters(char *starting_from) {
    if (starting_from == NULL) {
        return FALSE;
    }

    while (*starting_from != '\0') {
        if (*starting_from != ' ' && *starting_from != '\t') {
            return TRUE;
        }
        starting_from++;
    }

    return FALSE;
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


bool_t DoesInstructionExist(char *instruction){
   for (int i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
       if (strcmp(instruction, reserved_instructions[i]) == 0) {
           return TRUE;
       }
}


bool_t SymbolIsIllegal(char *symbol) {

    while (*symbol != '\0') {
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
        return true;
    }

    return false;
}

bool_t IsReservedName(char *name) {

    for (int i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
        if (strcmp(name, reserved_instructions[i]) == 0) {
            return TRUE;
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
        if (strcmp(name, register_names[i]) == 0) {
            return FALSE;
        }
    }

    return TRUE;
}

bool_t DirectiveDoesntExist(char *directive) {


    for (int i = 0; i < NUM_OF_DIRECTIVES; i++) {
        if (strcmp(directive, reserved_directives[i]) == 0) {
            return false;
        }
    }

    return true;
}

bool_t SymbolExceedCharacterLimit(char *symbol) {
    if (symbol == NULL) {
        return false;
    }

    int length = 0;
    while (*symbol != '\0') {
        length++;
        if (length > 31) {
            return true;
        }
        symbol++;
    }

    return false;
}
bool_t SymbolUsedAsAMacro(char *symbol, list_t *macro_list){
   if (null != find (symbol, symbolCompare, macro_list)){
	return TRUE;
   }  
   return FALSE;
}



static int SymbolCompare (void *str,void *key ){
   return (0 == strcmp ((char *) str, (char *) key ));  
}

bool_t CommaIsMissingInData(char *data){
   while (*data != '\0') {
	if (*data == ' ' || *data == '\t' ){
		if (*(data-1) != ','){
			return TRUE;
	}
   return FALSE;
}

