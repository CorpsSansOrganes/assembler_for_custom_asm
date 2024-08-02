#ifndef __ASSEMBLER__
#define __ASSEMBLER__

#include "utils.h"

result_t AssembleFiles(char *files[]);
/* 
 * For each file in files:
 * 1. preprocess
 * 2. first pass
 * 3. second pass
 * 4. create obj file.
 */

#endif /* __ASSEMBLER__ */
