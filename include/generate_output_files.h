#ifndef __GENERATE_OUTPUT_FILES__
#define __GENERATE_OUTPUT_FILES__

#include "utils.h"
#include "vector.h"
#include "symbol_table.h"

/* 
*@brief creating the obj file, entry file and extern file after the assembling process.
*@param opcode: the full opcode that the assembler generated.
*       symbol_table: the symbol table of the program.
*       input_path: the path to the input file.
*       IC: the instruction counter.
*       DC: the data counter.
*@return SUCCESS if the function finished the job successfullly. Otherwise, FAILURE;
*/


result_t GenerateOutputFiles (vector_t *opcode, symbol_table_t *symbol_table, char *input_path, int IC, int DC);

#endif __GENERATE_OUTPUT_FILES__