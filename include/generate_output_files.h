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
*@brief Creating the obj file, entry file and extern file after the assembling process.

*@param code_table  - The opcode of the code segment.
*       data_table - The opcode of the data segment
*       symbol_table - The symbol table of the program.
*       input_path - The path to the input file.
*       external_symbol_data_list - list of typedef external_symbol_data that 
*       contains the occurrences of each external symbol in the machine code
*       
*@return SUCCESS if the function finished the job successfullly. Otherwise, FAILURE;
*/

result_t GenerateOutputFiles(vector_t *code_table,
                             vector_t *data_table,
                             symbol_table_t *symbol_table,
                             const char *input_path,
                             ext_symbol_occurrences_t* ext_symbol_occurrences);

/*
 * @brief Initiates external symbol list.
 *
 * @return Upon success, return pointer to the list. Otherwise NULL. 
 */

ext_symbol_occurrences_t *CreateExternalSymbolList(void);

/*
 * @brief  Free all the resources that the external symbol list has used. 
 *
 * @param  ext_symbol_occurences - The list of the external symbol occurences.
 */

void DestroyExternSymbolList(ext_symbol_occurrences_t *ext_symbol_occurences);

/*
 * @brief  Add a listing for an occurrence where external symbol was used.
 *         
 *         If the symbol exists in the list, adds line to occurrences vector.
 *         Otherwise, create it & add a first occurence.
 *
 * @param  ext_symbol_occurences - List of the external symbol occurences.
 *         symbol_name - The symbol to search.
 *         IC - The address of the occurence.
 *         
 * @return SUCCESS if the function finished the job successfullly. Otherwise, MEM_ALLOCATION_ERROR.
 */
result_t AddExternalSymbolOccurence(ext_symbol_occurrences_t *ext_symbol_occurences,
                                    const char *symbol_name,
                                    unsigned int IC);

#endif /* __SH_ED_GENERATE_OUTPUT_FILES__ */
