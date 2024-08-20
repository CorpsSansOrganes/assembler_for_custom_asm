#ifndef __SH_ED_GENERATE_OPCODE__
#define __SH_ED_GENERATE_OPCODE__

#include "vector.h"
#include "language_definitions.h"

typedef enum {
  E,
  R,
  A
} encoding_type_t;

/* 
 * @brief Produce the machine code for an instruction statement.
 *
 * @param code_table - The vector that contains the code segment.
 *        instruction - The instruction to encode.
 *        source_operand - Pointer to the source operand, or NULL if the
 *          instruction doesn't take one.
 *        dest_operand - Pointer to the destination operand, or NULL if
 *          the instruction doesn't take one.
 *
 * @return SUCCESS if the memory machine were added to the code segment, 
 *         or MEM_ALLOCATION_ERROR upon a failure.
 */

result_t InstructionStatementToMachinecode(vector_t *code_table,
                                           const char *instruction,
                                           operand_t *source_operand,
                                           operand_t *dest_operand);

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

#endif /* __SH_ED_GENERATE_OPCODE__ */
