#ifndef __PRINT_ERRORS__
#define __PRINT_ERRORS__

#include "syntax_errors.h"
/*
 * @brief Prints error message according to the type of the error.
 * @param error_num - the enum number of the error
 *        line - the line which the error occured
 */

void PrintError(syntax_error_code_t error_num, int line);

#endif /* __PRINT_ERRORS__ */
