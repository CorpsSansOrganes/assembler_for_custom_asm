#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

/*
 * @brief Data structure representing a symbol table.
 */

typedef struct symbol_table symbol_table_t;
typedef struct symbol symbol_t;

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

int AddSymbol(symbol_table_t *table, symbol_t symbol);

/*
 * @brief Looks for entry in the symbol table by symbol name.
 */
symbol_t *FindSymbol(symbol_table_t *table, char *symbol_name);

#endif /* __SYMBOL_TABLE__ */
