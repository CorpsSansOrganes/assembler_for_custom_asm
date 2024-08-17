#ifndef __SYNTAX_ERRORS__
#define __SYNTAX_ERRORS__

#include "utils.h"
#include "language_definitions.h"
#include "symbol_table.h"
#include "list.h"
#include "preprocessing.h"

typedef struct {
  const char *file_name;
  unsigned int line_number;
  bool_t verbose;
} syntax_check_config_t;

/*
// ~~--~~--~~--~~--~~
// Preprocessor errors
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Checks if there are any characters after a certain point
 *   in a string, ignoring blank spaces (' ', '\t' etc). 
 *
 * Prints appropriate error message if a syntax error occurred.
 *
 * @param starting_from - Pointer to the point from which we check for
 *                        extrenuous characters.
 *
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if extra characters were detected, FALSE otherwise.
 */

bool_t DetectExtraCharacters(const char *starting_from,
                             syntax_check_config_t *config);

/*
 * @brief Checks if name is reserved by the language.
 *
 * Prints appropriate error message if a syntax error occurred.
 *
 * @param name - The name to check.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if is a reserved name, FALSE otherwise.
 */

bool_t IsReservedName(const char *name, syntax_check_config_t *config);

/*
 * @brief Checks if a macro has already been defined.
 *
 * Prints appropriate error message if a syntax error occurred.
 *
 * @param macro_name - The name to check
 *        table - The macro table to check.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if a macro with the name 'macro_name' was already defined, FALSE otherwise.
 */

bool_t MacroDefinedMoreThanOnce(const char *macro_name,
                                macro_table_t *table,
                                syntax_check_config_t *config);

/*
// ~~--~~--~~--~~--~~
// Instruction errors
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Checks if an instruction doesnt exist.
 *
 * Prints appropriate error message if a syntax error occurred.
 *
 * @param instruction - The suspected instruction.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the instruction doesnt exists, FALSE otherwise.
 */

bool_t InstructionDoesntExist(const char *instruction,
                              syntax_check_config_t *config);

/*
 * @brief Check if number of operands is correct for a given instruction.
 *
 * @param instruction - The operator
 *        num_of_operands - Number of operands handed to the operator.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 * 
 *
 * @return TRUE if num_of_operands isn't the expected number, FALSE otherwise.
 *
 *        NOTE: If the instruction isn't defined in the language's definition,
 *        behaviour of this function is undefined.
 */

bool_t WrongNumberOfOperands(const char *instruction,
                          int num_of_operands,
                          syntax_check_config_t *config);

/*
 * @brief Tell if an operand addressing method is viable for a given instruction.
 *
 *        For example, for "add r7, B", since direct register is a legal
 *        addressing method for add's source operand, the function would 
 *        return FALSE.
 *        However, since clr takes no source operand, calling the function 
 *        with any addressing method for a source operand would return TRUE.
 *        Similarly, "clr #2" is also illegal, as clr doesn't support immediate
 *        addressing, so the function would return TRUE.
 *
 *
 * @param instruction - The instruction performed.
 *        operand - The operand we check (e.g. '5', 'r0').
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the addressing method of the operand is illegal in the given
 *   instruction, or FALSE otherwise.
 *
 *        NOTE: If the instruction isn't defined in the language's definition,
 *        behaviour is undefined.
 *
 *        NOTE: If the operand is invalid (i.e. its addressing method is invalid)
 *        behaviour is undefined.
 */
bool_t IncorrectAddressingMethod(const char *instruction,
                                 const operand_t *operand,
                                 syntax_check_config_t *config);

/*
 * @brief Indicates if a symbol that has been encountered has been defined before.
 *        Used to check if there's an attempt to use a symbol that wasn't defined.
 *
  * @param symbol - The symbol which now been called
 *         table - The symbol table.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol never been defined previously, or FALSE otherwise.
 */

bool_t SymbolWasntDefined(char *symbol,
                          symbol_table_t *table,
                          syntax_check_config_t *config);

/*
 * @brief Checks if an an immediate operand contains a valid number that exceed
 *        maximum size to encode (2^11).
 *
 * @param operand - The operand
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 *        NOTE: if the operand isn't immediate behaviour is undefined.
 *        
 * @return TRUE if its too big. FALSE otherwise.
 */

bool_t ImmediateOperandTooBig (operand_t *operand, syntax_check_config_t *config);

/*
// ~~--~~--~~--~~--~~
// Symbol (label) errors
// ~~--~~--~~--~~--~~
*/

/*
 * @briefTell if theres been an attempt to define a symbol that have been defined previously 
 *
 * @param symbol - The symbol which now defined
 *        table - The symbol table.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol has already been defined, or FALSE otherwise.
 */
bool_t SymbolDefinedMoreThanOnce(char *symbol,
                                 symbol_table_t *table,
                                 syntax_check_config_t *config);



/*
 * @brief Check if a symbol that has been declared as .extern was previously 
 *        declared as .entry.
 *
 * @param symbol - The name of the symbol to check.
 *         table - The symbol table.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol was already defined as .entry, or FALSE otherwise.
 */

bool_t SymbolAlreadyDefinedAsEntry(char *symbol,
                                   symbol_table_t *table,
                                   syntax_check_config_t* config);

/*
 * @brief Check if a symbol that has been declared as .entry was previously 
 *        declared as .extern.
 *
 * @param symbol - The name of the symbol to check.
 *         table - The symbol table.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol was already defined as .extern, or FALSE otherwise.
 */

bool_t SymbolAlreadyDefinedAsExtern(char *symbol,
                                    symbol_table_t *table,
                                   syntax_check_config_t* config);

/*
 * @brief Tell if a symbol name is illegal.
 *
 * A symbol name is considered illegal if it:
 *   1. Starts with a non-alphabetical character.
 *   2. Has any illegal characters in it (must be alphabetical or
 *   numerical characters only).
 *   3. Exceeds character limit (32).
 *
  * @param symbol - The symbol (e.g. "SYMBOL")
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol is illegal, or FALSE otherwise.
 */

bool_t SymbolNameIsIllegal(const char *symbol, syntax_check_config_t *config);

/*
 * @brief Checks if a symbol name was already used as macro name.
 *
 * @param symbol - The symbol name to check.
 *        macros - The macro table.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *        
 * @return TRUE if symbol was already used as a macro. FALSE otherwise.
 */

bool_t SymbolUsedAsAMacro(char *symbol, macro_table_t *macros,
                          syntax_check_config_t *config);
/*
 * @brief Checks if a symbol definition was called without any definition.
 *        exapmle: SYMBOL: //Nothing after
 *
 * @param after_symbol - The line that comes after the symbol definition (i.e. after "SYMBOL:")
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *        
 * @return TRUE if the line is empty. FALSE otherwise.
 */

bool_t NoDefinitionForSymbol(const char *after_symbol, syntax_check_config_t *config);


/*
// ~~--~~--~~--~~--~~
// Directive errors
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Check if the directive that has been called does not exist. 
 * the existing directive are : .data , .string , .entry , .extern
 *
 * @param directive - Null-terminated string, which we want to check if its
 *                    a directive.
 *
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return INVALID_DIRECTIVE if the directive doesn't exists, or the directive type.
 */

directive_t DirectiveDoesntExist(const char *directive,
                                syntax_check_config_t *config);

/**
 * @brief Checks if `.data` definitions is correct. 
 * 
 *        This function examines the input string `data` to determine whether:
 *        1. Commas are properly placed between values in a `.data` definition. 
 *
 *        For example:
 *        13, 145 // OK
 *        13 145  // ERROR
 *
 *        2. No parameters are missing.
 *        
 *        For example:
 *        13, , 15 // ERROR
 *
 *        3. No illegal characters (for instance, alphabetical characters).
 *
 *        For example:
 *        13, 32x  // ERROR
 *
 *        4. There is atleast one parameter.
 *
 *        5. None of the data parameters are too big to encode (2^14).
 *
 * @param data - A pointer to the string containing the `.data` values to be checked.
 *               e.g.: "13, 143, 12".
 *        config - A pointer to the syntax checking configuration, which may include options 
 *                 for handling errors or warnings.
 *
 * @return `TRUE` if an error is encountered in `.data` definition, `FALSE` otherwise.
 */

bool_t IsIllegalDataParameter(const char *data, syntax_check_config_t *config);

/*
 * @brief Checks if a string is illegal.
 *        A string is legal if:
 *        1. it has ' " ' (quation marks) in the beginning and ending of the string.
 *        2. All of its characters are printable.
 *
 * @param str - The string to check.
 *        config - Pointer the configurations about the syntax check.
 *
 * @return TRUE if the string is illegal, FALSE otherwise.
 */

bool_t IsIllegalString(const char *str, syntax_check_config_t *config);

/*
 * @brief Checks if a parameter line passed to .extern or .entry directive
 *        is legal, i.e  each parameter recieved is a legal symbol name.
 *        
 *
 *        Function assumes no leading or trailing whitespaces.
 *
 * @param data - The parameter line to check (e.g. "SYMBOL1, SYMBOL2").
 *        config - Pointer the configurations about the syntax check.
 *        
 * @return TRUE if the parameter is illegal, FALSE otherwise.
 */

bool_t IsIllegalExternOrEntryParameter(const char *param,
                                       syntax_check_config_t *config);

/*
// ~~--~~--~~--~~--~~
// General syntax
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Tell if a register name doesn't exist.
 * the register names that exist are (r0 ... r7)
 *
 * @param register_name - The name of the register that been called.
 *        syntax_check_config - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if a register name doesn't exist, or FALSE otherwise.
 */

bool_t RegisterNameDoesntExist(const char *register_name,
                               syntax_check_config_t *config);

/*
// ~~--~~--~~--~~--~~
// Utilities
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Creates a new syntax_check_config_t object, containing configurations
 *        about the syntax check, which are used in the syntax checks. 
 *        In particular those configurations are used for optionally printing
 *        appropriate error messages.
 *
 * @param verbose - Determines whether to print an error or not.
 *        line number - The line number of the checked argument
 *        file name - The file that contains the checked argument.
 */

syntax_check_config_t CreateSyntaxCheckConfig(const char *file_name,
                                              unsigned int line_number,                                          
                                              bool_t verbose);


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
*         In particular, if the operand is detected to be invalid, the function
*         return INVALID.
*
*         NOTE: The function can't determine for any 'operand_name' whether
*         it is invalid or not. In particular, it can't tell if a name which
*         could be a symbol is actually a valid symbol.
*/

addressing_method_t DetectAddressingMethod(const char *operand_name);

#endif /* __SYNTAX_ERRORS__ */

