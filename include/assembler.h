#ifndef __SH_ED_ASSEMBLER__
#define __SH_ED_ASSEMBLER__

#include "utils.h"
#include "vector.h"
#include "macro_table.h"

/* Starting from the following address the program will be mapped. */
#define INITIAL_IC_VALUE 100

/*
 * @brief Assembler which takes a .am file and its correspoding macro table 
 *        (both produced by the preprocessor), and produces corresponding:
 *        1. object file (.ob)
 *        2. (Optionally) externals file (.ext)
 *        3. (Optionally) entries file (.ent)
 *
 * @param file_path - Path to the .am file to assemble.
 *        macro_list - The macro table produced by preprocessing stage for
 *                     the given file.
 *
 * @return Upon success, creates a .ob file and returns SUCCESS.
 *         Otherwise, an error code is returned.
 */

result_t AssembleFile(char *file_path,  macro_table_t *macro_list);

#endif /* __SH_ED_ASSEMBLER__ */
