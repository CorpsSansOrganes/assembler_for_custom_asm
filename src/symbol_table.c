#include <stdlib.h> /* malloc */
#include "symbol_table.h"
#include "list.h"
#include "utils.h"
#include "string.h"

typedef struct {
  const char *symbol_name;
  address_t address;
} symbol_t;

struct symbol_table {
  list_t *list;
};

static symbol_t *CreateSymbol(const char *symbol_name, address_t address);
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

result_t AddSymbol(symbol_table_t *table,
                   const char *symbol_name,
                   address_t address) {

  symbol_t *symbol = CreateSymbol(symbol_name, address);
  
  if (NULL == symbol) {
    return MEM_ALLOCATION_ERROR;
  }
  if (NULL == AddNode(table->list,  symbol)) {
    free(symbol);
    return MEM_ALLOCATION_ERROR;
  }
  return SUCCESS;
}

result_t FindSymbol(symbol_table_t *table,
                    const char *symbol_name,
                    address_t *address) {
  node_t *node = Find(table->list, SymbolCompare, (void *)symbol_name);
  if (node) {
    symbol_t *symbol = GetValue(node);
    *address = symbol->address;
    return SUCCESS;
  }
  else {
    *address = SYMBOL_NOT_FOUND;
    return FAILURE;
  }
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

static int SymbolCompare (void *symbol_ptr, void *key) {
  symbol_t *symbol= (symbol_t *)symbol_ptr;
  const char *symbol_name = symbol->symbol_name;
  return (0 == strcmp (symbol_name, (char *) key));
}

static symbol_t *CreateSymbol(const char *symbol_name, address_t address) {
  symbol_t *symbol = (symbol_t *)malloc(sizeof(symbol_t));
  if (NULL == symbol) {
    return NULL;
  }

  symbol->symbol_name = symbol_name;
  symbol->address = address;
  return symbol;
}
