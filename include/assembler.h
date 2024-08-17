#ifndef __ASSEMBLER_SH_ED__
#define __ASSEMBLER_SH_ED__

#include "utils.h"
#include "vector.h"
#include "macro_table.h"

/* Starting from the following address the program will be mapped. */
#define INITIAL_IC_VALUE 100

typedef struct external_symbol_data {
  const char *symbol_name;
  vector_t *occurences;
} external_symbol_data_t;

/*
 * @brief Assembler which takes a .am file and its correspoding macro table 
 *        (both produced by the preprocessor), and produces corresponding:
 *        1. object file (.ob)
 *        2. (Optionally) externals file (.ext)
 *        3. (Optionally) entries file (.ent)
 *
 * @param file_path - Path to the .am file to assemble.
 *        macro_list - The macro table 
 * @return Upon success, creates a .ob file and returns SUCCESS.
 *         Otherwise, an error code is returned.
 */

result_t AssembleFile(char *file_path,  macro_table_t *macro_list);

#endif /* __ASSEMBLER_SH_ED__ */
