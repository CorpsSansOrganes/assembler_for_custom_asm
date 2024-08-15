#include <stdlib.h> /* malloc */
#include <stdio.h> /* perror */
#include <assert.h> /* assert */
#include "symbol_table.h"
#include "list.h"
#include "utils.h"
#include "string.h"

struct symbol_struct {
  const char *symbol_name;
  symbol_type_t type;
  address_t address;
  symbol_memory_area_t area;
};

struct symbol_table {
  list_t *list;
};


static result_t AddSymbolWithType(symbol_table_t *table,
                                 const char *symbol_name,
                                 address_t address,
                                 symbol_type_t type,
                                 symbol_memory_area_t area);

static symbol_t *CreateSymbol(const char *symbol_name,
                              address_t address,
                              symbol_type_t type,
                              symbol_memory_area_t area);

static int SymbolCompare(void *symbol_ptr, void *key);


symbol_table_t *CreateSymbolTable(void) {
  symbol_table_t *new_symbol_table = (symbol_table_t *)malloc(sizeof(symbol_table_t));
  if (NULL == new_symbol_table) {
    perror("Error allocating memory for a symbol table");
    return NULL;
  }

  new_symbol_table->list = CreateList();
  if (NULL == new_symbol_table->list) {
    perror("Error allocating memory for a symbol table");
    free(new_symbol_table);
    return NULL;
  }

  return new_symbol_table;
}

void DestroySymbolTable(symbol_table_t *table){
  node_t *node = GetHead(table->list);
  assert(table); 
  
  while (NULL != node) {
    free(GetValue(node));
    node = GetNext(node);
  }

  DestroyList(table->list);
  free(table);
}

result_t AddSymbol(symbol_table_t *table,
                   const char *symbol_name,
                   address_t address,
                   symbol_memory_area_t area) {
  assert(table); assert(symbol_name);

  return AddSymbolWithType(table, symbol_name, address, REGULAR, area);

}

result_t AddExternalSymbol(symbol_table_t *table,
                           const char *symbol_name) {

  assert(table); assert(symbol_name);

  return AddSymbolWithType(table,
                           symbol_name,
                           0,  /* Address doesn't matter w/ external symbols */
                           EXTERN,
                           0); /* Code/data segment doesn't matter also. */
}

result_t ChangeSymbolToEntry(symbol_table_t *table,
                             const char *symbol_name) {

  symbol_t *symbol = FindSymbol(table, symbol_name);
  assert(table); assert(symbol_name);

  if (NULL == symbol) {
    return FAILURE;
  }
  symbol->type = ENTRY;

  return SUCCESS;
}

void UpdateSymbolAddress (symbol_t *symbol ,address_t new_address) {
  assert(symbol);
  symbol->address = new_address;
}

symbol_t *FindSymbol(symbol_table_t *table,
                    const char *symbol_name) {

  node_t *node = NULL; 
  assert(table); assert(symbol_name);

  node = Find(table->list, SymbolCompare, (void *)symbol_name);
  if (NULL == node) {
    return NULL;
  }

  return (symbol_t *)GetValue(node);
}

const char *GetSymbolName(symbol_t *symbol) {
  assert(symbol);
  return symbol->symbol_name;
}

symbol_type_t GetSymbolType(symbol_t *symbol) {
  assert(symbol);
  return symbol->type;
}

symbol_memory_area_t GetSymbolMemoryArea (symbol_t *symbol) {
  assert(symbol);
  return symbol->area;
}

address_t GetSymbolAddress(symbol_t *symbol) {
  assert(symbol); 
  return symbol->address;
}

/*
 * Problem: User gets symbol, which is the value of the node, not 
 *          the node itself.
symbol_t *GetNextSymbol(symbol_t *symbol) {
    return GetNext(symbol);  
}

symbol_t *GetHeadSymbol (symbol_table_t *symbol_table) {
  return GetHead(symbol_table->list);
}
*/

static int SymbolCompare(void *symbol_ptr, void *key) {
  symbol_t *symbol = (symbol_t *)symbol_ptr;
  const char *symbol_name = symbol->symbol_name;

  return (0 == strcmp (symbol_name, (char *) key));
}

static symbol_t *CreateSymbol(const char *symbol_name,
                              address_t address,
                              symbol_type_t type,
                              symbol_memory_area_t area) {
  symbol_t *symbol = (symbol_t *)malloc(sizeof(symbol_t));
  if (NULL == symbol) {
    perror("Error allocating memory for a new symbol\n");
    return NULL;
  }

  symbol->symbol_name = symbol_name;
  symbol->address = address;
  symbol->type = type;
  symbol->area = area;
  return symbol;
}

static result_t AddSymbolWithType(symbol_table_t *table,
                         const char *symbol_name,
                         address_t address,
                         symbol_type_t type,
                         symbol_memory_area_t area) {
  symbol_t *symbol = CreateSymbol(symbol_name, address, type, area);
  
  if (NULL == symbol) {
    return MEM_ALLOCATION_ERROR;
  }

  if (NULL == AddNode(table->list,  symbol)) {
    fprintf(stderr, 
            "Error adding new symbol %s to symbol table\n",
            symbol_name);
    free(symbol);
    return MEM_ALLOCATION_ERROR;
  }

  return SUCCESS;
}

