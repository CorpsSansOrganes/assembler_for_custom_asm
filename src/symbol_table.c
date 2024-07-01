#include "list.h"

struct {
  char *symbol_name;
  int mem_address;
} symbol;

struct {
  symbol_t cur;
  symbol_t *next;
} symbol_table;
