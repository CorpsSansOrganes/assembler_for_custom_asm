#ifndef __SH_ED_GENERATE_OUTPUT_FILES__
#define __SH_ED_GENERATE_OUTPUT_FILES__

#include "utils.h"
#include "vector.h"
#include "symbol_table.h"
#include "list.h"

/*
 * @brief All occurrence where an external symbol was used.
 */
typedef struct external_symbol_data {
  const char *symbol_name;
  vector_t *occurences;
} external_symbol_data_t;

/* 
 * @brief Creating the obj file, entry file and extern file after the assembling process.
 * @param opcode - The full opcode that the assembler generated.
 *       symbol_table - The symbol table of the program.
 *       input_path - The path to the input file.
 *       ext_symbol_occurrences - List of external_symbol_data_t that contains
 *        the occurences of each external symbol in the machine code
 *
 * @return SUCCESS if the function finished the job successfullly. Otherwise, FAILURE.
 */

result_t GenerateOutputFiles (vector_t *code_table,
                              vector_t *data_table,
                              symbol_table_t *symbol_table,
                              char *input_path,
                              list_t* ext_symbol_occurrences);

#endif /* __SH_ED_GENERATE_OUTPUT_FILES__ */
