#ifndef __SYNTAX_ERRORS__
#define __SYNTAX_ERRORS__

#include "utils.h"
#include "language_definitions.h"
#include "symbol_table.h"
#include "list.h"

// ~~--~~--~~--~~--~~
// Preprocessor errors
// ~~--~~--~~--~~--~~

/*
 * @brief Checks if there are any characters after a certain point
 *   in a string, ignoring blank spaces (' ', '\t' etc).
 *
 * @param starting_from - Pointer to the point from which we check for
 *   extrenuous characters.
 *
 * @return TRUE if extra characters were detected, FALSE otherwise.
 */
bool_t DetectExtraCharacters(char *starting_from);

/*
 * @brief Checks if name is reserved by the language.
 *
 * @param name - The name to check.
 *
 * @return TRUE if is a reserved name, FALSE otherwise.
 */
bool_t IsReservedName(char *name);

// ~~--~~--~~--~~--~~
// Instruction errors
// ~~--~~--~~--~~--~~

/*
 * @brief Checks if an instruction exist.
 *
 * @param instruction - The suspected instruction.
 *
 * @return TRUE if the instruction exists, FALSE otherwise.
 */
bool_t DoesInstructionExist(char *instruction);

/*
 * @brief Check if number of operands is correct.
 *
 * @param instruction - The operator
 *        num_of_operands - Number of operands handed to the operator.
 * 
 * @return -1 if too little operands, 0 if the correct number of operands or 
 *         1 if too many operands.
 */
int CorrectNumberOfOperands(char *instruction, int num_of_operands);

/*
 * @brief Tell if an operand addressing method is viable for a given instruction.
 *
 * @param instruction - The instruction performed.
 *        operand - The operand we check (e.g. '5', 'r0').
 *        type - Destination or source operand.
 *
 * @return TRUE if the addressing method of the operand is illegal in the given
 *   instruction, or FALSE otherwise.
 */
bool_t IncorrectAddressingMethod(char *instruction, char *operand, operand_type_t type);

// ~~--~~--~~--~~--~~
// Symbol (label) errors
// ~~--~~--~~--~~--~~

/*
 * @brief Tell if a symbol defined more than once, i.e if a symbol already exists in the symbol table.
 *
 * @param symbol - The symbol which now defined
 *        table - The symbol table.
 *
 * @return TRUE if the symbol has already been defined, or FALSE otherwise.
 */
bool_t SymbolDefinedMoreThanOnce(char *symbol, symbol_table_t *table);

/*
 * @brief Tell if a symbol have been defined previously in symbol table before it got called
 *
  * @param symbol - The symbol which now been called
 *         table - The symbol table.
 *
 * @return TRUE if the symbol never been defined previously, or FALSE otherwise.
 */

bool_t SymbolWasntDefined(char *symbol, symbol_table_t *table);

/*
 * @brief Tell if a symbol have been directed with both .extern and .entry directives
 *
  * @param symbol - The symbol that been checked
 *         table - The symbol table.
 *
 * @return TRUE if the symbol got both directives, or FALSE otherwise.
 */

bool_t SymbolIsBothExternAndEntry(char *symbol, symbol_table_t *table);


/*
 * @brief Tell if a symbol has illegal characters in it (must be alphabetical or numerical characters only)
 *
  * @param symbol - The symbol.
 *
 * @return TRUE if the symbol is illegal, or FALSE otherwise.
 */

bool_t SymbolIsIllegal(char *symbol);

/*
 * @brief Tell if a symbol begins with illegal characters (must begin with alphabetical characters only)
 *
  * @param symbol - The symbol.
 *
 * @return TRUE if the symbol is illegal, or FALSE otherwise.
 */

bool_t SymbolPrefixIllegal(char *symbol);

/*
 * @brief Tell if the length of the symbol name exceeded the character limit (31).
 *
  * @param symbol - The symbol.
 *
 * @return TRUE if the symbol exceeded the character limit, or FALSE otherwise.
 */

bool_t SymbolExceedCharacterLimit(char *symbol);

/*
 * @brief Tell if a symbol name already used as a macro.
 *
  * @param symbol - The symbol 
 *         macro_list - The list of macros.
 *
 * @return TRUE if the symbol name already used as a macro, or FALSE otherwise.
 */

bool_t SymbolUsedAsAMacro(char *symbol, list_t *macro_list);

// ~~--~~--~~--~~--~~
// Directive errors
// ~~--~~--~~--~~--~~


/*
 * @brief Tell if the directive that been called does not exist (directive always starts with '.'). 
 * the existing directive are : .data , .string , .entry , .extern
 *
  * @param directive - The directive that have been called.
 *
 * @return TRUE if the dorective doesn't exist, or FALSE otherwise.
 */

bool_t DirectiveDoesntExist(char *directive);
/*
 * @brief Tell if the directive is exist but have been called with uppercase letters instead of lowercase letters
 *
  * @param directive - The directive that have been called.
 *
 * @return TRUE if the directive have been called with uppercase letters , or FALSE otherwise.
 */

bool_t DirectiveIsUpperCase(char *directive);

/*
 * @brief Tell if the .data directive line that been performed is missing commas between the parameters. 
 *
  * @param data - The .data directive line that have been performed.
 *
 * @return TRUE if the .data directive line is missing commas between the parameters, or FALSE otherwise.
 */

bool_t CommaIsMissingInData(char *data);


// ~~--~~--~~--~~--~~
// General syntax
// ~~--~~--~~--~~--~~

/*
 * @brief Tell if a register name doesn't exist.
 * the register names that exist are (r0 ... r7)
 *
  * @param register_name - The name of the register that been called
 *
 * @return TRUE if a register name doesn't exist, or FALSE otherwise.
 */

bool_t RegisterNameDoesntExist(char *register_name);

#endif /* __SYNTAX_ERRORS__ */

