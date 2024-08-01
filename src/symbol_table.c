#include "symbol_table.h"
#include "list.h"

struct {
  char *symbol_name;
  int address;
} symbol;

struct {
  list_t *list; // EXAMPLE: using list as the underlying ds.
} symbol_table;
