#include <stdio.h> /* perror */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcpy */
#include "preprocessing.h"
#include "syntax_errors.h"
#include "string_utils.h"
#include "list.h"

struct macro_struct {
  const char *macro_name;
  const char *macro_definition;
};

struct macro_table {
  list_t *list;
};

/* ~~--~~--~~--~~--~~
  Static function declarations
 ~~--~~--~~--~~--~~ */
static macro_table_t *CreateMacroTable(void);
static result_t AddMacroIfUnique(macro_table_t *table,
                          const char *macro_name,
                          const char *macro_definition);
static macro_t *CreateMacro(const char *macro_name, const char *macro_definition);
static int CmpMacro(void *macro, void *key);

/* ~~--~~--~~--~~--~~
  Public functions
 ~~--~~--~~--~~--~~ */
macro_table_t *PreprocessFile(char *input_path, char *output_path) {
  bool_t should_write_line = TRUE;
  int line_number = 1;
  int total_errors = 0;
  char line[MAX_LINE_SIZE];
  FILE *input_file = NULL;
  FILE *output_file = NULL;
  macro_table_t *table = CreateMacroTable();

  if (NULL == table) {
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a macro table\n");
    return NULL;
  }

  input_file = fopen(input_path, "r");
  if (NULL == input_file) {
    perror("Couldn't open input file");
    DestroyMacroTable(table);
    return NULL;
  }

  output_file = fopen(output_path, "w");
  if (NULL == output_file) {
    perror("Couldn't open output file");
    fclose(input_file);
    DestroyMacroTable(table);
    return NULL;
  }

  while (NULL != fgets(line, MAX_LINE_SIZE, input_file)) {
    if(IsComment(line)) {
      should_write_line = FALSE;
    }
    else if (IsMacro(line)) {
      if (FAILURE == ReadMacro(input_file)) {

      }
    }
  }

  /*
  Open file.
  For each line in file:
    If (IsNewMacro(line)):
        1. Check that name isn't reserved.
        2. Create new entry (AddMacroIfUnique(macros, line))
        3. If error occured, inc. total_errors.
        4. Skip to the end of the macro macro_definition.
      
    Else 
      to_Write = line
      If (IsMacro(line)):
            1. Find macro name in the macros.
            2. to_write = macro_definition for the macro
          
      Write to_write to buffer / new_file
  Write a new file.
  */

  /*
   * Syntax errors to check:
   * 1. No extra characters at the end of macr or endmacr line.
   * 2. Macro name isn't a reserved name.
   */
  return table;
}

void DestroyMacroTable(macro_table_t *table) {
  node_t *node = GetHead(table->list);
  while (NULL != node) {
    free(GetValue(node));
    node = GetNext(node);
  }

  DestroyList(table->list);
  free(table);
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

const char *GetMacroDefinition(macro_t *macro) {
  return macro->macro_definition;
}

/*
 * @brief Creates a new empty macro table.
 * @return Upon success, a new macro table. When failure, returns NULL.
 */
static macro_table_t *CreateMacroTable(void) {
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

/*
 * @brief Adds a new macro, if one with the same name doesn't exist in
 *        the table.
 *
 * @param table - The table into which we wish to insert a new macro.
 *        macro_name - The name of the macro we wish to create.
 *        macro_definition - The definition of the macro (i.e. what the macro
 *                     expends to).
 *
 * @return If there's another macro whose name is macro_name, 
 *         FAILURE is returned.
 *         If a memory error occurs, MEM_ALLOCATION_ERROR is returned.
 *         Else if the macro has been written to the table successfully, 
 *         SUCCESS is returned.
 */
static result_t AddMacroIfUnique(macro_table_t *table,
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

static macro_t *CreateMacro(const char *macro_name,
                            const char *macro_definition) {
  macro_t *macro = (macro_t *)malloc(sizeof(macro_t));
  char *name = NULL;
  char *definition = NULL;
  size_t length = 0;

  if (NULL == macro) {
    return NULL;
  }

  length = strlen(macro_name);
  name = (char *)malloc(length);
  if (NULL == name) {
    free(macro);
    return NULL;
  }

  length = strlen(macro_definition);
  definition = (char *)malloc(length);
  if (NULL == definition) {
    free(macro);
    free(name);
    return NULL;
  }

  strcpy(name, macro_name); 
  strcpy(definition, macro_definition); 
  return macro;
}

static int CmpMacro(void *value, void *key) {
  macro_t *macro = (macro_t *)value;
  return (0 == strcmp(macro->macro_name, (const char *)key));
}
