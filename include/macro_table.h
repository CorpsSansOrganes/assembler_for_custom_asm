#ifndef __SH_ED_MACRO_TABLE__
#define __SH_ED_MACRO_TABLE__

/*
 * @brief Data structure representing a macro table.
 *
 *        Each macro is defined by two attributes: (1) name, (2) definition.
 *        The definition is essentially what the macro should expand to.
 *        For example:
 *
 *        ```ASM
 *        macr m_macr
 *          inc r2
 *          mov A, r1
 *        endmacr
 *        ```
 *
 *        The macro name is 'm_macr', and the definition is:
 *          'inc r2
 *          mov A, r1'
 */

#include "utils.h"
typedef struct macro_table macro_table_t;
typedef struct macro_struct macro_t;

/*
 * @brief Creates a new empty macro table.
 * @return Upon success, a new macro table. When failure, returns NULL.
 */
macro_table_t *CreateMacroTable(void);

/*
 * @brief Adds a new macro, if one with the same name doesn't exist in
 *        the table.
 *
 * @param table - The table into which we wish to insert a new macro.
 *        macro_name - The name of the macro we wish to create.
 *        macro_definition - The definition of the macro (i.e. what the macro
 *                     expends to).
 *
 * @return If there's another macro whose name is macro_name, 
 *         FAILURE is returned.
 *         If the required memory couldn't be allocated for the macro,
 *         MEM_ALLOCATION_ERROR is returned.
 *         Else, if the macro has been written to the table successfully, 
 *         SUCCESS is returned.
 *
 * NOTE: the function performs a deep copy of macro_name & macro_definition.
 */

result_t AddMacroIfUnique(macro_table_t *table,
                          const char *macro_name,
                          const char *macro_definition);

/* @brief Same as a AddMacroIfUnique, without checking if a macro of the
 *        same name exists.
 *
 * @return Same as AddMacroIfUnique, but FAILURE isn't returned.
 */

result_t AddMacro(macro_table_t *table,
                  const char *macro_name,
                  const char *macro_definition);

/*
 * @brief Deallocates the memory of a macro table, including macro names &
 *        definitions.
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
 * @return The macro's definition, or NULL if it cant find the macro.
 */
const char *GetMacroDefinition(macro_t *macro);

#endif /* __SH_ED_MACRO_TABLE__ */
