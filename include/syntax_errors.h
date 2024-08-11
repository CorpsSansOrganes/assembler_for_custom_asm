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
 * @brief Checks if there are any characters after a certain point
 *   in a string, ignoring blank spaces (' ', '\t' etc).
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
 * @param name - The name to check.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if is a reserved name, FALSE otherwise.
 */

bool_t IsReservedName(const char *name, syntax_check_config_t *config);

/*
// ~~--~~--~~--~~--~~
// Instruction errors
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Checks if an instruction doesnt exist.
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
 * @param instruction - The instruction performed.
 *        operand - The operand we check (e.g. '5', 'r0').
 *        type - Destination or source operand.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the addressing method of the operand is illegal in the given
 *   instruction, or FALSE otherwise.
 *
 *        NOTE: If the instruction isn't defined in the language's definition,
 *        behaviour of this function is undefined.
 */
bool_t IncorrectAddressingMethod(const char *instruction,
                                 const operand_t *operand,
                                 operand_type_t type,
                                 syntax_check_config_t *config);

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
 * @brief Tell if a symbol that been directed as .extern was already defined as .entry
 *
 * @param symbol - The symbol that been checked.
 *         table - The symbol table.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol was already defined as .entry, or FALSE otherwise.
 */

bool_t SymbolAlreadyDefinedAsEntry(char *symbol,
                                   symbol_table_t *table,
                                   syntax_check_config_t* config);

/*
 * @brief Tell if a symbol that been directed as .entry was already defined as .extern
 *
 * @param symbol - The symbol that been checked.
 *         table - The symbol table.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol was already defined as .extern, or FALSE otherwise.
 */

bool_t SymbolAlreadyDefinedAsExtern(char *symbol,
                                    symbol_table_t *table,
                                   syntax_check_config_t* config);

/*
 * @brief Tell if a symbol has illegal characters in it (must be alphabetical or
 * numerical characters only)
 *
  * @param symbol - The symbol (without the colon at the end).
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol is illegal, or FALSE otherwise.
 */

bool_t SymbolNameIsIllegal(const char *symbol, syntax_check_config_t *config);

/*
 * @brief Tell if a symbol begins with illegal characters 
 *        (must begin with alphabetical characters only)
 *
 * @param symbol - The symbol name.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol is illegal, or FALSE otherwise.
 */

bool_t SymbolPrefixIllegal(const char *symbol,
                           syntax_check_config_t *config);

/*
 * @brief Check if the length of the symbol name exceeded the character limit (31).
 *
 * @param symbol - The symbol.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol exceeded the character limit, or FALSE otherwise.
 */

bool_t SymbolExceedCharacterLimit(const char *symbol,
                                  syntax_check_config_t *config);

/*
 * @brief Check if a symbol name already used as a macro.
 *
  * @param symbol - The symbol 
 *         macro_list - The list of macros.
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the symbol name already used as a macro, or FALSE otherwise.
 */

bool_t SymbolUsedAsAMacro(char *symbol, macro_table_t *macro_list,
                          syntax_check_config_t *config);

/*
// ~~--~~--~~--~~--~~
// Directive errors
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Check if the directive that been called does not exist. 
 * the existing directive are : .data , .string , .entry , .extern
 *
 * @param directive - Null-terminated string, which we want to check if its
 *                    a directive.
 *
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the directive doesn't exist, or FALSE otherwise.
 */

bool_t DirectiveDoesntExist(const char *directive,
                            syntax_check_config_t *config);

/*
 * @brief Checks if the .data directive line that been performed is missing commas
 *        between the parameters. assuming the line is linted
 *
 * @param data - The .data directive line that have been performed (starting 
 *        from the first parameter, i.e not including the word '.data ' itself).
 *
 *        config - Configurations about the syntax check (see CreateSyntaxCheckConfig)
 *
 * @return TRUE if the .data directive line is missing commas between the parameters,
 *         or FALSE otherwise.
 */

bool_t CommaIsMissingInData(char *data, syntax_check_config_t *config);

/*
 * @brief Tell if the line that been performed is missing paramters, 
 *        i.e there is two commas in a row or there is a comma in the end of the line without parameter that follows it.
 *        assuming the line is linted
 *
 * @param data - The .data directive line that have been performed (starting 
 *        from the first parameter, i.e not including the directive/instruction itself).
 *        syntax_check_config - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if the .data directive line is missing parameters,
 *         or FALSE otherwise.
 */

bool_t ParameterIsMissingInlLine(char *line,
                                 syntax_check_config_t *config);

/*
 * @brief Tell if the .data directive line contains paramters which are not numbers.
 *
 * @param data - The .data directive line that have been performed (starting 
 *        from the first parameter, i.e not including the directive itself).
 *        syntax_check_config - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if the .data directive line is contains illegal parameters, or FALSE otherwise;
 */

bool_t IllegalParametersInData(char *line,
                               syntax_check_config_t *config);

/*
 * @brief Checks if a string is illegal. a string is legal if it has ' " ' (quation marks) in the beginning and ending of the string.
 *
 * @param str - The string to check.
 *        config - Pointer the configurations about the syntax check.
 *
 * @return TRUE if the string is illegal, FALSE otherwise.
 */

bool_t IsIllegalString(char *str, syntax_check_config_t *config);


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

bool_t RegisterNameDoesntExist(char *register_name,
                               syntax_check_config_t *config);

#endif /* __SYNTAX_ERRORS__ */

