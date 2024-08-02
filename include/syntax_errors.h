#ifndef __SYNTAX_ERRORS__
#define __SYNTAX_ERRORS__

#include "utils.h"
#include "language_definitions.h"

// Preprocessor errors

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
 * @param name - The nane to check.
 *
 * @return TRUE if is a reserved name, FALSE otherwise.
 */
bool_t IsReservedName(char *name);

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

#endif /* __SYNTAX_ERRORS__ */

