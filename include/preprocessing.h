#ifndef __PREPROCESSING__
#define __PREPROCESSING__

/*
 * @brief Utilities for usage in the preprocessing stage.
 *        The preprocessor receives a .as file, and outputs a
 *        .am file.
 *
 *        The preprocessor finds macro usage, and swaps them
 *        for their definitions.
 *
 */

#include "list.h"
#include "error.h"

/*
 * @brief Performs preprocessing on a .as file:
 *  1. Indentifying macro definitions, and swaps macro for
 *     their definitions.
 *  2. Detect errors in macros.
 *
 * @param file_path - Path to the .as input file to be processed.
 * @returns 
 * 1. If no errors where detected, a corresponding .am file is created,
 *    and the function returns 0.
 * 2. If an error (or several errors) occur, the errors are printed to stdout,
 *    and the function returns 1
 *
 */
int PreprocessFile(char *file_path);


#endif /* __PREPROCESSING__ */
