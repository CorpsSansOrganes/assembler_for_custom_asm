/* macro_table.c
 *
 * This module implements the macro_table data structure along with its associated utilities, 
 * which are similar to those of the list data structure.
 */




#include <stdlib.h> /* malloc, free */
#include <string.h> /* strlen, strcpy */
#include <stdio.h> /* fprintf */
#include "macro_table.h"
#include "list.h"
#include "string_utils.h"

struct macro_struct {
  const char *macro_name;
  const char *macro_definition;
};

struct macro_table {
  list_t *list;
};

static macro_t *CreateMacro(const char *macro_name, const char *macro_definition);
static int CmpMacro(void *macro, void *key);

macro_table_t *CreateMacroTable(void) {
  macro_table_t *new_macro_table = (macro_table_t *)malloc(sizeof(macro_table_t));
  if (NULL == new_macro_table) {
    return NULL;
  }

  new_macro_table->list = CreateList();
  if (NULL == new_macro_table->list) {
    free(new_macro_table);
    return NULL;
  }

  return new_macro_table;
}

result_t AddMacroIfUnique(macro_table_t *table,
                          const char *macro_name,
                          const char *macro_definition) {

  if (NULL != FindMacro(table, macro_name)) {
    return FAILURE;
  }

  return AddMacro(table, macro_name, macro_definition);
}

void DestroyMacroTable(macro_table_t *table) {
  node_t *node = GetHead(table->list);
  while (NULL != node) {
    macro_t *macro = GetValue(node);
    free((void *)(macro->macro_name));
    free((void *)(macro->macro_definition));
    free(macro);
    node = GetNext(node);
  }

  DestroyList(table->list);
  free(table);
}

result_t AddMacro(macro_table_t *table,
                  const char *macro_name,
                  const char *macro_definition) {

  macro_t *macro = CreateMacro(StrDup (macro_name), StrDup (macro_definition));

  if (NULL == macro) {
    return MEM_ALLOCATION_ERROR;
  }

  AddNode(table->list, macro);
  return SUCCESS;
}

const char *GetMacroDefinition(macro_t *macro) {
  return macro->macro_definition;
}

macro_t *FindMacro(macro_table_t *table,
                   const char *macro_name) {
  node_t *node = Find(table->list, CmpMacro, (void *)macro_name);
  if (node) {
    macro_t *macro = GetValue(node);
    return macro;
  }
  else {
    return NULL;
  }
}

static macro_t *CreateMacro(const char *macro_name,
                            const char *macro_definition) {
  macro_t *macro = (macro_t *)malloc(sizeof(macro_t));

  if (NULL == macro) {
    fprintf(stderr,
            "CreateMacro: Couldn't allocate memory for new macro '%s'\n",
            macro_name);
    return NULL;
  }

  macro->macro_definition = macro_definition;
  macro->macro_name = macro_name;

  return macro;
}

static int CmpMacro(void *value, void *key) {
  macro_t *macro = (macro_t *)value;
  return (0 == strcmp(macro->macro_name, (const char *)key));
}

/* TODO: delete */
void PrintAllMacros(macro_table_t *table) {
  node_t *iter = GetHead(table->list);
  while (NULL != iter) {
    macro_t *macro = GetValue(iter);
    printf("macro name: %s\n", macro->macro_name);
    printf("macro definition:\n %s\n\n", macro->macro_definition);
  }
}
