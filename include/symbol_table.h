#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

/*
 * @brief Data structure representing a symbol table.
 *
 *        A symbol has 3 key attributes: (1) name, (2) address and (3) type.
 *        The symbol's name acts as its key.
 */

#include "utils.h"

typedef struct symbol_table symbol_table_t;
typedef int address_t;

typedef enum {
  EXTERN,
  ENTRY,
  REGULAR
} symbol_type_t;

typedef struct symbol_struct symbol_t;

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
result_t AddSymbol(symbol_table_t *table,
                   const char *symbol_name,
                   address_t address);

/*
 * @brief Add a symbol to the table as an external symbol.
 * @param table - The table we wish to add a symbol to.
 *        symbol_name - The symbol's key.
 *
 * @return SUCCESS if successful, or an error code upon failure.
 */
result_t AddExternalSymbol(symbol_table_t *table,
                           const char *symbol_name);

/*
 * @brief finds the symbol by his key and changes his type to entry.
 * @param table - The table which contains the symbol.
 *        symbol_name - The symbol's key.
 *
 * @return SUCCESS if successful, or an error code upon failure.
 */
result_t ChangeSymbolToEntry(symbol_table_t *table,
                       const char *symbol_name);

/*
 * @brief Looks for entry in the symbol table by symbol name.
 * @param table - The symbol table in which we search.
 *        symbol_name - The key of the symbol we're looking for.
 *
 * @return If a symbol with that name is found, it's returned. Otherwise NULL.
 */
symbol_t *FindSymbol(symbol_table_t *table,
                    const char *symbol_name);

/*
 * @brief Tell the type of a symbol.
 * @return Symbol's type.
 */
symbol_type_t GetSymbolType(symbol_t *symbol);

/*
 * @brief Tell the address of a symbol.
 * @return Symbol's address.
 */
address_t GetSymbolAddress(symbol_t *symbol);

#endif /* __SYMBOL_TABLE__ */
