#ifndef __ASSEMBLER__
#define __ASSEMBLER__

#include "utils.h"

/*
 * @brief Assembler which takes .am files (after preprocessing)
 *        and converts them to corresponding object files.
 * @param file_path - Path to the .am file to assemble.
 * @return Upon success, creates a .ob file and returns SUCCESS.
 *         Otherwise, an error code is returned.
 */

result_t AssembleFile(char *file_path);
/* 
 * For each file in files:
 * 1. first pass
 * 2. second pass
 * 3. create obj file.
 */

#endif /* __ASSEMBLER__ */
