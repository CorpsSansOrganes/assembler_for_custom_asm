#ifndef __PRINT_ERRORS__
#define __PRINT_ERRORS__

/* 
* an enum containing all of the error messages
*/
typedef enum {
RESERVED_WORD_ERROR,
EXTRA_CHARACTERS_MACRO_ERROR,
INSTRUCTION_NOT_EXIST_ERROR,
NOT_ENOUGH_OPERATORS_ERROR,
TOO_MANY_OPERATORS_ERROR,
WRONG_ADDRESSING_METHOD_ERROR,
REGISTER_NAME_NOT_EXIST_ERROR,
SYMBOL_ALREADY_DEFINED_ERROR,
WRONG_COLON_SYNTAX_ERROR,
LABEL_EXCEEDED_LENGTH_ERROR,
LABEL_PREFIX_ERROR,
ILLEGAL_LABEL_CHARACTERS_ERROR,
DIRECTIVE_NOT_EXIST_ERROR,
UPPERCASE_DIRECTIVE_ERROR,
MISSING_COMMA_ERROR,
SYMBOL_NOT_EXIST_ERROR
} syntax_error_code_t;

/*
 * @brief prints error message according to the type of the error.
 * @param error_num - the enum number of the error
	  line - the line which the error occured
 */

void print_error (syntax_error_code_t error_num,int line);

#endif /* __PRINT_ERRORS__ */
