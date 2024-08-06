#ifndef __PREPROCESSING__
#define __PREPROCESSING__

/*
 * @brief Utilities for usage in the preprocessing stage.
 *        The preprocessor receives a .as file, and outputs a
 *        .am file.
 *
 *        The preprocessor:
 *        1. Finds macro usage, and swaps them for their definitions.
 *        2. Removes comments.
 *        3. Removes blank lines.
 *        4. Removes extraneous spaces, tabs, etc.
 */

#include "utils.h"

#define MAX_LINE_SIZE 100

typedef struct macro_table macro_table_t;
typedef struct macro_struct macro_t;

/*
 * @brief Performs preprocessing on a .as file:
 *  1. Indentifying macro definitions, and swaps macro for
 *     their definitions.
 *  2. Detect errors in macros.
 *  3. Remove comments, blank lines, extraneous spaces & tabs.
 *
 *  Assumptions made about macro are:
 *  1. No nested macros.
 *  2. Every macro definition has an endmacr line.
 *  3. Macros are always defined before they're used.
 *
 *  Additionally, we assume that each line in the input line doesn't exceed
 *  MAX_LINE_SIZE.
 *
 * @param input_path - Path to the .as input file to be processed.
 *        output_path - Path to the .am output file.
 *
 * @returns 
 * 1. If no errors where detected, a corresponding .am file is created,
 *    a macro table with the macros which were present in the file (this table might be empty).
 * 2. If an error (or several errors) occur, the errors are printed to stdout,
 *    and NULL is returned.
 *
 */
macro_table_t *PreprocessFile(char *input_path, char *output_path);


/*
 * @brief Deallocates the memory of a macro table.
 * @param table - the table we wish to destroy.
 */
void DestroyMacroTable(macro_table_t *table);

/*
 * @brief Looks for entry in the macro table by macro name.
 * @param table - The macro table in which we search.
 *        macro_name - The key of the macro we're looking for.
 *
 * @return If a macro with that name is found, it's returned. Otherwise NULL.
 */
macro_t *FindMacro(macro_table_t *table,
                   const char *macro_name);

/*
 * @brief Returns the definition for a given macro 
 * @param macro - The macro whose definition we're looking for.
 *
 * @return The macro's definition.
 */
const char *GetMacroDefinition(macro_t *macro);

#endif /* __PREPROCESSING__ */
