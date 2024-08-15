#ifndef __ASSEMBLER_SH_ED__
#define __ASSEMBLER_SH_ED__

#include "utils.h"

#define INITIAL_IC_VALUE 100

/*
 * @brief Assembler which takes .am files (after preprocessing) and the macro table of the file 
 *        and converts it to corresponding object files.
 * @param file_path - Path to the .am file to assemble.
 *        macro_list - The macro table 
 * @return Upon success, creates a .ob file and returns SUCCESS.
 *         Otherwise, an error code is returned.
 */

result_t AssembleFile(char *file_path,  macro_table_t *macro_list);
/* 
 * For each file in files:
 * 1. first pass
 * 2. second pass
 * 3. create obj, ext and ent file.
 */

#endif /* __ASSEMBLER_SH_ED__ */