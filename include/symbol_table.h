#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

#define SYMBOL_NOT_FOUND -1

/*
 * @brief Data structure representing a symbol table.
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
 * @brief Add a symbol to the table as an entry symbol.
 * @param table - The table we wish to add a symbol to.
 *        symbol_name - The symbol's key.
 *
 * @return SUCCESS if successful, or an error code upon failure.
 */
result_t AddEntrySymbol(symbol_table_t *table,
                       const char *symbol_name);

/*
 * @brief Looks for entry in the symbol table by symbol name.
 * @param table - The symbol table in which we search.
 *        symbol_name - The key of the symbol we're looking for.
 *        address - In which the address will be placed if found.
 * @return SUCCESS if a symbol with that name is found, and address will be set
 *         to the symbol's address.
 *         FAILURE otherwise, and address will be set to SYMBOL_NOT_FOUND.
 */
result_t FindSymbol(symbol_table_t *table,
                    const char *symbol_name,
                    address_t *address);

/*
 * @brief Tells if symbol is of a certain type.
 * @param table - The symbol table in which we search for the symbol.
 *        symbol_name
 */
bool_t IsType(symbol_table_t *table,
              const char *symbol_name,
              symbol_type_t type);

#endif /* __SYMBOL_TABLE__ */
