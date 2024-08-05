#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__
#define SYMBOL_NOT_FOUND -1
/*
 * @brief Data structure representing a symbol table.
 */

#include "utils.h"

typedef struct symbol_table symbol_table_t;
typedef int address_t;

/*
 * @brief Creates a empty symbol table.
 * @return Upon success, return a pointer to the newly created symbol table.
 *         Upon failure, return NULL.
 */
symbol_table_t *CreateSymbolTable(void);

/*
 * @brief Deallocates the memory of a symbol table.
 * @param table - the table we wish to destroy.
 */
void DestroySymbolTable(symbol_table_t *table);

/*
 * @brief Add a new symbol to the table.
 * @param table - The table we wish to add a symbol to.
 *        symbol_name - The symbol's key.
 *        address - Memory address of the symbol.
 *
 * @return SUCCESS if successful, or an error code upon failure.
 */
result_t AddSymbol(symbol_table_t *table, char *symbol_name, address_t address);

/*
 * @brief Looks for entry in the symbol table by symbol name.
 * @param table - The symbol table in which we search.
 *        symbol_name - The key of the symbol we're looking for.
 *        address - In which the address will be placed if found.
 * @return SUCCESS if a symbol with that name is found, FAILURE otherwise.
 */
result_t FindSymbol(symbol_table_t *table, char *symbol_name, address_t *address);

#endif /* __SYMBOL_TABLE__ */
