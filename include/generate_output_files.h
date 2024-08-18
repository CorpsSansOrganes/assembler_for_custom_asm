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
  vector_t *occurrences;
} external_symbol_data_t;

typedef struct {
  list_t *external_symbols;
} ext_symbol_occurrences_t;

/* 
*@brief creating the obj file, entry file and extern file after the assembling process.
*@param code_opcode: the opcode of the code segment.
*       data_opcode: the opcode of the data segment
*       symbol_table: the symbol table of the program.
*       input_path: the path to the input file.
*       external_symbol_data_list: list of typedef external_symbol_data that contains the occurrences of each external symbol in the machine code
*       IC: the instruction counter.
*       DC: the data counter.
*@return SUCCESS if the function finished the job successfullly. Otherwise, FAILURE;
*/

result_t GenerateOutputFiles(vector_t *code_table,
                             vector_t *data_table,
                             symbol_table_t *symbol_table,
                             const char *input_path,
                             ext_symbol_occurrences_t* ext_symbol_occurrences);

ext_symbol_occurrences_t *CreateExternalSymbolList();
void DestroyExternSymbolList(ext_symbol_occurrences_t *ext_symbol_occurences);

/*
 * If the symbol exists in the list, add line to occurrences vector.
 * Otherwise, create it & add a first occurence.
 */
result_t AddExternalSymbolOccurence(ext_symbol_occurrences_t *ext_symbol_occurences, const char *symbol_name, unsigned int line);

#endif /* __GENERATE_OUTPUT_FILES__ */
