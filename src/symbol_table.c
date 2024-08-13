#include <stdlib.h> /* malloc */
#include "symbol_table.h"
#include "list.h"
#include "utils.h"
#include "string.h"

struct symbol_struct {
  const char *symbol_name;
  address_t address;
  symbol_type_t type;
};

struct symbol_table {
  list_t *list;
};

static result_t AddSymbolWithType(symbol_table_t *table,
                         const char *symbol_name,
                         address_t address,
                         symbol_type_t type);

static symbol_t *CreateSymbol(const char *symbol_name,
                              address_t address,
                              symbol_type_t type);

static int SymbolCompare (void *symbol_ptr, void *key);

symbol_table_t *CreateSymbolTable(void) {
  symbol_table_t *new_symbol_table = (symbol_table_t *)malloc(sizeof(symbol_table_t));
  if (NULL == new_symbol_table) {
    return NULL;
  }

  new_symbol_table->list = CreateList();
  if (NULL == new_symbol_table->list) {
    free(new_symbol_table);
    return NULL;
  }

  return new_symbol_table;
}

void DestroySymbolTable(symbol_table_t *table){
  node_t *node = GetHead(table->list);
  while (NULL != node) {
    free(GetValue(node));
    node = GetNext(node);
  }

  DestroyList(table->list);
  free(table);
}

result_t AddSymbol(symbol_table_t *table,
                   const char *symbol_name,
                   address_t address) {
  return AddSymbolWithType(table, symbol_name, address, REGULAR);

}

result_t AddExternalSymbol(symbol_table_t *table,
                           const char *symbol_name) {
  return AddSymbolWithType(table, symbol_name, 0, EXTERN);
}

result_t ChangeSymbolToEntry(symbol_table_t *table,
                       const char *symbol_name) {
  symbol_t *symbol;
  symbol = FindSymbol ( table, symbol_name);
  if (NULL == symbol){
    return FAILURE;
  }
  symbol->type = ENTRY;
  return SUCCESS;
}

symbol_t *FindSymbol(symbol_table_t *table,
                    const char *symbol_name) {
  node_t *node = Find(table->list, SymbolCompare, (void *)symbol_name);
  if (node) {
    symbol_t *symbol = GetValue(node);
    return symbol;
  }
  else {
    return NULL;
  }
}

symbol_type_t GetSymbolType(symbol_t *symbol) {
  return symbol->type;
}

address_t GetSymbolAddress(symbol_t *symbol) {
  return symbol->address;
}

static int SymbolCompare (void *symbol_ptr, void *key) {
  symbol_t *symbol = (symbol_t *)symbol_ptr;
  const char *symbol_name = symbol->symbol_name;
  return (0 == strcmp (symbol_name, (char *) key));
}

static symbol_t *CreateSymbol(const char *symbol_name,
                              address_t address,
                              symbol_type_t type) {
  symbol_t *symbol = (symbol_t *)malloc(sizeof(symbol_t));
  if (NULL == symbol) {
    return NULL;
  }

  symbol->symbol_name = symbol_name;
  symbol->address = address;
  symbol->type = type;
  return symbol;
}


static result_t AddSymbolWithType(symbol_table_t *table,
                         const char *symbol_name,
                         address_t address,
                         symbol_type_t type) {
  symbol_t *symbol = CreateSymbol(symbol_name, address, type);
  
  if (NULL == symbol) {
    return MEM_ALLOCATION_ERROR;
  }
  if (NULL == AddNode(table->list,  symbol)) {
    free(symbol);
    return MEM_ALLOCATION_ERROR;
  }
  return SUCCESS;
}
