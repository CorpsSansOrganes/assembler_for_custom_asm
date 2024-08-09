#ifndef __SYNTAX_ERRORS__
#define __SYNTAX_ERRORS__

#include "utils.h"
#include "language_definitions.h"
#include "symbol_table.h"
#include "list.h"
#include "preprocessing.h"

typedef struct {
  unsigned int line_number;
  bool_t verbose;
  const char *file_name;
} syntax_check_info_t;

/*
// ~~--~~--~~--~~--~~
// Preprocessor errors
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Checks if there are any characters after a certain point
 *   in a string, ignoring blank spaces (' ', '\t' etc).
 *
 * @param starting_from - Pointer to the point from which we check for
 *   extrenuous characters.
 *   syntax_check_info - struct that contains the parameters : 
 *      verbose - determines whether to print an error or not.
 *      line number - the line number of the checked argument
 *      file name - the file that contains the checked argument.

 * @return TRUE if extra characters were detected, FALSE otherwise.
 */

bool_t DetectExtraCharacters(const char *starting_from, syntax_check_info_t syntax_check_info);

/*
 * @brief Checks if name is reserved by the language.
 *
 * @param name - The name to check.
 *      syntax_check_info - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if is a reserved name, FALSE otherwise.
 */

bool_t IsReservedName(char *name, syntax_check_info_t syntax_check_info);

/*
// ~~--~~--~~--~~--~~
// Instruction errors
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Checks if an instruction exist.
 *
 * @param instruction - The suspected instruction.
 *      syntax_check_info - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if the instruction exists, FALSE otherwise.
 */
bool_t InstructionDoesntExist(char *instruction, syntax_check_info_t syntax_check_info);

/*
 * @brief Check if number of operands is correct for a given instruction line.
 *
 * @param instruction - The operator
 *      num_of_operands - Number of operands handed to the operator.
 *      syntax_check_info - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 * 
 * @return the number of operands minus the number of needed operands.
 *         thus, return 0 if it fits. 
 */
int WrongNumberOfOperands(char *instruction, int num_of_operands, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if an operand addressing method is viable for a given instruction.
 *
 * @param instruction - The instruction performed.
 *        operand - The operand we check (e.g. '5', 'r0').
 *        type - Destination or source operand.
 *        syntax_check_info - struct that contains the parameters : 
 *              verbose - determines whether to print an error or not.
 *              line number - the line number of the checked argument
 *              file name - the file that contains the checked argument.
 *
 * @return TRUE if the addressing method of the operand is illegal in the given
 *   instruction, or FALSE otherwise.
 */
bool_t IncorrectAddressingMethod(char *instruction,
                                 char *operand,
                                 operand_type_t type, syntax_check_info_t syntax_check_info);

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
 *        syntax_check_info - struct that contains the parameters : 
 *              verbose - determines whether to print an error or not.
 *              line number - the line number of the checked argument
 *              file name - the file that contains the checked argument.
 *
 * @return TRUE if the symbol has already been defined, or FALSE otherwise.
 */
bool_t SymbolDefinedMoreThanOnce(char *symbol, symbol_table_t *table, syntax_check_info_t syntax_check_info);
/*
 * @brief Tell if a symbol was used without colon or with space between the symbol and the colon
 *
 * @param symbol - The symbol which now been called
 *        syntax_check_info - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if a symbol was used without colon or with space between the symbol and the colon, or FALSE otherwise.
 */

bool_t ColonSyntaxError(char *symbol, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if a theres been an attempt to use a symbol that havent been defined previously
 *
  * @param symbol - The symbol which now been called
 *         table - The symbol table.
 *         syntax_check_info - struct that contains the parameters : 
 *              verbose - determines whether to print an error or not.
 *              line number - the line number of the checked argument
 *              file name - the file that contains the checked argument.
 *
 * @return TRUE if the symbol never been defined previously, or FALSE otherwise.
 */


bool_t SymbolWasntDefined(char *symbol, symbol_table_t *table, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if a symbol that been directed as .extern was already defined as .entry
 *
 * @param symbol - The symbol that been checked.
 *         table - The symbol table.
 *         syntax_check_info - struct that contains the parameters : 
 *              verbose - determines whether to print an error or not.
 *              line number - the line number of the checked argument
 *              file name - the file that contains the checked argument.
 *
 * @return TRUE if the symbol was already defined as .entry, or FALSE otherwise.
 */

bool_t SymbolAlreadyDefinedAsEntry(char *symbol, symbol_table_t *table, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if a symbol that been directed as .entry was already defined as .extern
 *
 * @param symbol - The symbol that been checked.
 *         table - The symbol table.
 *         syntax_check_info - struct that contains the parameters : 
 *              verbose - determines whether to print an error or not.
 *              line number - the line number of the checked argument
 *              file name - the file that contains the checked argument.
 *
 * @return TRUE if the symbol was already defined as .extern, or FALSE otherwise.
 */

bool_t SymbolAlreadyDefinedAsExtern(char *symbol, symbol_table_t *table, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if a symbol has illegal characters in it (must be alphabetical or numerical characters only)
 *
  * @param symbol - The symbol (without the colon at the end).
  *        syntax_check_info - struct that contains the parameters : 
 *              verbose - determines whether to print an error or not.
 *              line number - the line number of the checked argument
 *              file name - the file that contains the checked argument.
 *
 * @return TRUE if the symbol is illegal, or FALSE otherwise.
 */

bool_t SymbolIsIllegal(char *symbol, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if a symbol begins with illegal characters (must begin with alphabetical characters only)
 *
 * @param symbol - The symbol.
 *        syntax_check_info - struct that contains the parameters : 
 *              verbose - determines whether to print an error or not.
 *              line number - the line number of the checked argument
 *              file name - the file that contains the checked argument.
 *
 * @return TRUE if the symbol is illegal, or FALSE otherwise.
 */

bool_t SymbolPrefixIllegal(char *symbol, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if the length of the symbol name exceeded the character limit (31).
 *
 * @param symbol - The symbol.
 *        syntax_check_info - struct that contains the parameters : 
 *              verbose - determines whether to print an error or not.
 *              line number - the line number of the checked argument
 *              file name - the file that contains the checked argument.
 *
 * @return TRUE if the symbol exceeded the character limit, or FALSE otherwise.
 */

bool_t SymbolExceedCharacterLimit(char *symbol, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if a symbol name already used as a macro.
 *
  * @param symbol - The symbol 
 *         macro_list - The list of macros.
 *         syntax_check_info - struct that contains the parameters : 
 *              verbose - determines whether to print an error or not.
 *              line number - the line number of the checked argument
 *              file name - the file that contains the checked argument.
 *
 * @return TRUE if the symbol name already used as a macro, or FALSE otherwise.
 */

bool_t SymbolUsedAsAMacro(char *symbol, macro_table_t *macro_list, syntax_check_info_t syntax_check_info);

/*
// ~~--~~--~~--~~--~~
// Directive errors
// ~~--~~--~~--~~--~~
*/

/*
 * @brief Tell if the directive that been called does not exist (directive always starts with '.'). 
 * the existing directive are : .data , .string , .entry , .extern
 *
 * @param directive - The directive that have been called.
 *        syntax_check_info - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if the directive doesn't exist, or FALSE otherwise.
 */

bool_t DirectiveDoesntExist(char *directive, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if the directive is exist but have been called with uppercase letters instead of lowercase letters
 *
 * @param directive - The directive that have been called.
 *        syntax_check_info - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if the directive have been called with uppercase letters , or FALSE otherwise.
 */

bool_t DirectiveIsUpperCase(char *directive, syntax_check_info_t syntax_check_info);

/*
 * @brief Tell if the .data directive line that been performed is missing commas
 *        between the parameters. assuming the line is linted
 *
 * @param data - The .data directive line that have been performed (starting 
 *        from the first parameter, i.e not including the word '.data ' itself).
 *        syntax_check_info - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if the .data directive line is missing commas between the parameters,
 *         or FALSE otherwise.
 */

bool_t CommaIsMissingInData(char *data, syntax_check_info_t syntax_check_info);

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
 *        syntax_check_info - struct that contains the parameters : 
 *          verbose - determines whether to print an error or not.
 *          line number - the line number of the checked argument
 *          file name - the file that contains the checked argument.
 *
 * @return TRUE if a register name doesn't exist, or FALSE otherwise.
 */

bool_t RegisterNameDoesntExist(char *register_name, syntax_check_info_t syntax_check_info);

#endif /* __SYNTAX_ERRORS__ */

