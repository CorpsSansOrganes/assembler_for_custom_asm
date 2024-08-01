#include "symbol_table.h"
#include "list.h"
#include "utils.h"
#include "string.h"
struct {
  char *symbol_name;
  int address;
} symbol;

struct {
  list_t *list;
} symbol_table;

symbol_table_t *CreateSymbolTable(void){
  symbol_table_t *new_symbol_table =  (new_symbol_table *)malloc(sizeof(new_symbol_table));
  if (NULL == new_symbol_table){
	return NULL;
}
  symbol_table_t->list = CreateList();
  if (NULL == new_symbol_table->list){
	DestroySymbolTable(new_symbol_table);
	return NULL;
  }

  return symbol_table_t;
}

result_t AddSymbol(symbol_table_t *table, symbol_t *symbol){

  if (NULL == AddNode(table->list,  symbol){
	return FAILURE;
  }
  return SUCCESS;
}
address_t *FindSymbol(symbol_table_t *table, char *symbol_name){
  address_t address =  (address_t *)malloc(sizeof(address_t));
  node_t *node = Find(table->list, SymbolCompare, symbol_name);
  if (NULL != node){
	address	= (address_t) GetValue(node);
	return address;
}
  address =  (address_t) SYMBOL_NOT_FOUND;
  return address;
}

void DestroySymbolTable(symbol_table_t *table){
  if (NULL == table){
	return;
}
  DestroyList (table->list);
  free(table);
}

static int SymbolCompare (void *str,void *key ){
	return (0 == strcmp ((char *) str, (char *) key ));  
}
