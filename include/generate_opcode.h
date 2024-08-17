#include "vector.h"
#include "language_definitions.h"

typedef enum {
  E,
  R,
  A,
} encoding_type_t;

/* 
*@brief produce the opcode of an instruction line, and adds the correct number to the instruction counter
*
*@param parameters: the operands of the instruction
*       instruction_word: the instruction word
*
*@return  vector containing 1-3 bitmap words 
*/

vector_t *InstructionLineToMachinecode(operand_t *first_operand, operand_t *second_operand, unsigned int num_of_operands, char *instruction_name, int *IC);

/* 
 * @brief Produce the memory encoding of an .string directive statement and add 
 *        them to the data segment.
 *
 * @param data_table - The vector that contains the data segment.
 *        string - The string to convert to machine code, with quotations marks.
 *                 e.g. "\"hello world!\"". 
 *
 *        NOTE: The function assumes that the string passed is legal.
 *
 * @return SUCCESS if the memory machine were added to the data segment, 
 *         or MEM_ALLOCATION_ERROR upon a failure.
 */

result_t StringDirectiveToMachinecode(vector_t *data_table, char *string);

/* 
 * @brief Produce the memory encoding of an .data directive statement and add
 *        them to the data segment.
 *
 * @param data_table - The vector that contains the data segment.
 *        params - A string containing the parameters passed to the .data directive.
 *                 e.g. "+13, 18, 0,-1,+333"
 *
 *        NOTE: The function assumes that the params string passed is legal.
 *
 * @return SUCCESS if the memory machine were added to the data segment, 
 *         or MEM_ALLOCATION_ERROR upon a failure.
 *
 */

result_t DataDirectiveToMachinecode(vector_t *data_table, char *params);
