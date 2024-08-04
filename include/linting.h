#ifndef __LINTING__
#define __LINTING__

#include "utils.h"

/*
 * @brief Cosmetic touch-up of an input file.
 *        1. Remove comments.
 *        2. Remove blank lines.
 *        3. Remove extraneous spaces, tabs, etc.
 *
 * @param input_path - Path to the input file.
 *        output_path - Path to the output file.
 *
 * @return SUCCESS upon success, or an appropriate error code.
 */
result_t LintFile(char *input_path, char *output_path);

#endif /* __LINTING__ */
