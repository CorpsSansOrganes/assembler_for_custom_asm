#include <stdlib.h> /* malloc, free */
#include <string.h> /* strlen, strcpy */
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

  macro_t *macro = NULL;
  if (NULL != FindMacro(table, macro_name)) {
    return FAILURE;
  }

  macro = CreateMacro(macro_name, macro_definition);
  if (NULL == macro) {
    return MEM_ALLOCATION_ERROR;
  }

  AddNode(table->list, macro);
  return SUCCESS;
}

void DestroyMacroTable(macro_table_t *table) {
  node_t *node = GetHead(table->list);
  while (NULL != node) {
    macro_t *macro = GetValue(node);
    free((void *)macro->macro_definition);
    free((void *)macro->macro_name);
    free(macro);
    node = GetNext(node);
  }

  DestroyList(table->list);
  free(table);
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
    return NULL;
  }
  macro->macro_definition = StrDup(macro_definition);
  if (NULL == macro->macro_definition) {
    free(macro); macro = NULL;
    return NULL;
  }

  macro->macro_name = strdup(macro_name);
  if (NULL == macro->macro_name) {
    free(macro); macro = NULL;
    free((void *)macro_definition); macro_definition = NULL;
    return NULL;
  }

  return macro;
}

static int CmpMacro(void *value, void *key) {
  macro_t *macro = (macro_t *)value;
  return (0 == strcmp(macro->macro_name, (const char *)key));
}

