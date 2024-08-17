#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

/*
 * @brief Data structure representing a symbol table.
 *
 *        A symbol is any label, e.g.:
 *        HELLO: mov #4, r1
 *        HEY: .data 13, -953, 17
 *
 *        In essence, it is a symbolic representation of a memory address.
 *        This memory address may be in the data or the code segments, depending on whether
 *        the label is of a .string or .data directive or an instruction statement.
 *        
 *        A symbol has 4 key attributes: (1) name, (2) address, (3) type and (4) memory area.
 *        The symbol's name acts as its key.
 */

#include "utils.h"
#include "list.h"

typedef struct symbol_table symbol_table_t;
typedef int address_t;


typedef enum {
  EXTERN,
  ENTRY,
  REGULAR
} symbol_type_t;

typedef enum {
  CODE,
  DATA
} symbol_memory_area_t;


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
 *        area - memory area of the symbol (Data/Code)
 *
 * @return SUCCESS if successful, or an error code upon failure.
 */
result_t AddSymbol(symbol_table_t *table,
                   const char *symbol_name,
                   address_t address,
                   symbol_memory_area_t area);

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
 * @brief Finds the symbol by his key and changes his type to entry.
 * @param table - The table which contains the symbol.
 *        symbol_name - The symbol's key.
 *
 * @return SUCCESS if successful, FAILURE if a symbol with that name isn't found.
 */
result_t ChangeSymbolToEntry(symbol_table_t *table,
                       const char *symbol_name);


/*
 * @brief updates the address of a symbol.
 * @param symbol - Pointer to the symbol we wish to update.
 *        new_address - The new address.
 */

void UpdateSymbolAddress (symbol_t *symbol ,address_t new_address);

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
 * @brief Get a symbol's name.
 * @return Symbol's name.
 */

const char *GetSymbolName(symbol_t *symbol);

/*
 * @brief Get a symbol's type
 * @return Symbol's type.
 */

symbol_type_t GetSymbolType(symbol_t *symbol);

/*
 * @brief Tell the memory area of a symbol.
 * @return Symbol's area.
 */

symbol_memory_area_t GetSymbolMemoryArea (symbol_t *symbol);

/*
 * @brief Tell the address of a symbol.
 * @return Symbol's address.
 */
address_t GetSymbolAddress(symbol_t *symbol);

/*
 * @brief Return a symbol table as a list of symbols.
 * @param table - The symbol we wish to turn into a list.
 * @return A list where the value of each node is a pointer to a symbol.
 */

list_t *AsList(symbol_table_t *table);

#endif /* __SYMBOL_TABLE__ */

