/* generate_output_files.c
 *
 * This module generates the following output files:
 * - .ob
 * - .ext (if needed)
 * - .ent (if needed)
 *
 * Additionally, it defines and implements the structure 
 * that holds occurrences of external symbols for the .ext file.
 */



#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "bitmap.h"
#include "vector.h"
#include "symbol_table.h"
#include "assembler.h"
#include "preprocessing.h"
#include "string_utils.h"
#include "language_definitions.h"
#include "generate_output_files.h"


static result_t WriteHeader (char *output_path, int IC, int DC);
static result_t GenerateEntriesFile (symbol_table_t *symbol_table, char *output_path);
static result_t GenerateOBJFile (vector_t *code_opcode, vector_t *data_opcode, char *output_path);
static result_t GenerateExternFile (symbol_table_t *symbol_table, char *output_path,ext_symbol_occurrences_t *external_symbol_data_list);
static int ExternalSymbolCompare (void *value, void *key);

#define BIT_MASK_15_BITS (0x7FFF)

ext_symbol_occurrences_t *CreateExternalSymbolList() {
  ext_symbol_occurrences_t *ext_list = malloc(sizeof(ext_symbol_occurrences_t));

  if (NULL == ext_list){
    perror ("Error: Couldn't allocate memory for external symbols lists\n");
    return NULL;
  }

  ext_list->external_symbols = CreateList();
  if (NULL == ext_list->external_symbols) {
    perror ("Error: Couldn't allocate memory for external symbols lists\n");
    free(ext_list);
    return NULL;
  }

  return ext_list;
}

result_t AddExternalSymbolOccurence(ext_symbol_occurrences_t *ext_symbol_occurrences,
                                    const char *symbol_name,
                                    unsigned int line) {

  external_symbol_data_t *external_symbol_data = NULL;
  node_t *node = Find(ext_symbol_occurrences->external_symbols,
                      ExternalSymbolCompare,
                      (void *)symbol_name);

  /* If this is the first occurrence */
  if (NULL == node) {
    external_symbol_data = malloc(sizeof(external_symbol_data_t *));
    if (NULL == external_symbol_data) {
      return MEM_ALLOCATION_ERROR;
    }

    external_symbol_data->symbol_name = StrDup(symbol_name);
    if (NULL == external_symbol_data->symbol_name) {
      perror("Error duplicating name");
      free(external_symbol_data);
      return MEM_ALLOCATION_ERROR;
    }

    external_symbol_data->occurrences = CreateVector(3, sizeof(unsigned int));
    if (NULL == external_symbol_data->occurrences) {
      perror ("Error creating vector");
      free((void *)external_symbol_data->symbol_name);
      free(external_symbol_data);
      return MEM_ALLOCATION_ERROR;
    }

    if (NULL == AddNode (ext_symbol_occurrences->external_symbols,
                         external_symbol_data)) {
      perror ("Error creating vector");
      free((void *)external_symbol_data->symbol_name);
      free(external_symbol_data->occurrences);
      free(external_symbol_data);
      return MEM_ALLOCATION_ERROR;
    }
  }

  /* If this is not the first occurrence */
  else {
    external_symbol_data = (external_symbol_data_t *)GetValue(node);
  }

  if (SUCCESS != AppendVector(external_symbol_data->occurrences, &line)) {
    perror ("Error appending vector");
    return MEM_ALLOCATION_ERROR;
  }

  return SUCCESS;
}

void DestroyExternSymbolList(ext_symbol_occurrences_t *ext_symbol_occurrences) {
  external_symbol_data_t *external_symbol_data = NULL;
  node_t *node = GetHead(ext_symbol_occurrences->external_symbols);

  while (NULL != node) {
    external_symbol_data = GetValue(node);
    DestroyVector(external_symbol_data->occurrences);
    free((void *)external_symbol_data->symbol_name);
    free(external_symbol_data);
    node = GetNext(node);
  }

  DestroyList(ext_symbol_occurrences->external_symbols);
}

result_t GenerateOutputFiles(vector_t *code_table,
                             vector_t *data_table,
                             symbol_table_t *symbol_table,
                             const char *input_path,
                             ext_symbol_occurrences_t* ext_symbol_occurrences) {

  bool_t error_occurred = FALSE;
  char *path = NULL;
  size_t length = strlen(input_path);

  path = (char *)malloc((length + 1) * sizeof(char));
  if (NULL == path) {
    perror("Couldn't allocate string for output paths\n");
    return MEM_ALLOCATION_ERROR;
  }

  /* Copy path except file extension */
  strncpy(path, input_path, length - 2);

  /* Generate .ob file */
  strcpy(path + (length - 2), "ob");

  if (SUCCESS != GenerateOBJFile(code_table, data_table, path)) {
    error_occurred = TRUE;
  }

  /* Generate .ext file */
  strcpy(path + (length - 2), "ext");
  if (SUCCESS != GenerateExternFile(symbol_table,
                                         path,
                                         ext_symbol_occurrences)) {
    error_occurred = TRUE;
  }

  /* Generate .ent file */
  strcpy(path + (length - 2), "ent");
  if (SUCCESS != GenerateEntriesFile(symbol_table, path)) {
    error_occurred = TRUE;
  }

  free(path);
  return error_occurred ? FAILURE :SUCCESS;
}

static result_t GenerateOBJFile (vector_t *code_table,
                                 vector_t *data_table,
                                 char *output_path) {
  int cur_mem_address = INITIAL_IC_VALUE;
  int i = 0;
  /* .ob format is mainly 2 columns:
   * (1) address - memory address, one word at a time */
  char *address = (char *) malloc (15 * sizeof(char));

  /* (2) bitmap - content of that memory address */
  char *bitmap =  (char *) malloc (15 * sizeof(char));
  char *str_to_write = (char *) malloc (15 * sizeof(char));
  FILE *obj_file = NULL;
  bitmap_t opcode_line = 0;

  /* Before the 2 columns, we write total code & data symbols. */
  if (SUCCESS != WriteHeader(output_path,
                             GetSizeVector(code_table),
                             GetSizeVector(data_table))) {
    free (address);
    free (bitmap);
    free (str_to_write);
    return FAILURE;
  }

  obj_file = fopen(output_path, "a");
  if (NULL == obj_file) {
    free(address);
    free(bitmap);
    free(str_to_write);
    perror("Couldn't open obj file");
    return ERROR_OPENING_FILE; 
  }

  /* First comes the code segment... */
  for (i = 0; i < GetSizeVector(code_table); i++) {
    opcode_line = *(bitmap_t *)GetElementVector(code_table, i);
    /* Ignoring bits bigger than word size */
    opcode_line &= BIT_MASK_15_BITS;
    sprintf(address, "%04d", cur_mem_address);
    sprintf(bitmap, "%05o", (unsigned int)opcode_line);
    sprintf(str_to_write, "%s %s\n", address, bitmap);
    if (EOF == fputs(str_to_write, obj_file)) {
      perror("Error writing to file");
      fclose(obj_file);
      free(address);
      free(bitmap);
      free(str_to_write);
      return ERROR_WRITING_TO_FILE;
    }

    cur_mem_address++;
  }

  /* ... then comes data segment */
  for (i = 0; i < GetSizeVector(data_table); i++) {
    opcode_line = *(bitmap_t *)GetElementVector(data_table, i);
    /* Ignoring bits bigger than word size */
    opcode_line &= BIT_MASK_15_BITS;
    sprintf(address, "%04d", cur_mem_address);
    sprintf(bitmap, "%05o", (unsigned int)opcode_line);
    sprintf(str_to_write, "%s %s\n", address, bitmap);
    if (EOF == fputs(str_to_write, obj_file)) {
      perror("Error writing to file");
      fclose(obj_file);
      free(address);
      free(bitmap);
      free(str_to_write);
      return ERROR_WRITING_TO_FILE;
    }

    cur_mem_address++;
  }

  free (address);
  free (bitmap);
  free (str_to_write);
  fclose(obj_file);
  return SUCCESS;
}


static result_t GenerateEntriesFile (symbol_table_t *symbol_table, char *output_path){
  FILE *entry_file = NULL;
  char *str_to_write = NULL;
  node_t *iter = (node_t *)GetHead(AsList(symbol_table));

  /* No symbols, no files to write */
  if (NULL == iter) {
    return SUCCESS;
  }

  /* Allocate resources */
  str_to_write = (char *) malloc (MAX_LINE_LENGTH*sizeof(char));
  if (NULL == str_to_write) {
    fprintf(stderr, "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;   
  }

  /* Find first .entry symbol (if one exists), and create .ent file only then */
  while (NULL != iter) {
    symbol_t *symbol = GetValue(iter);

    if (ENTRY == GetSymbolType(symbol)) {
      entry_file = fopen(output_path, "a");
      if (NULL == entry_file) {
        free(str_to_write);
        perror("Couldn't open entry file");
        return ERROR_OPENING_FILE; 
      }
      break;
    }
    iter = GetNext(iter);
  }

  while (NULL != iter) {
    symbol_t *symbol = GetValue(iter);

    if (ENTRY == GetSymbolType(symbol)) {
      sprintf (str_to_write, "%s %d\n",
               GetSymbolName(symbol),
               GetSymbolAddress(symbol));
      if (EOF == fputs(str_to_write, entry_file)) {
        fclose (entry_file);
        free (str_to_write);
        perror("Error writing to file");
        return ERROR_WRITING_TO_FILE;
      }
    }
    iter = GetNext(iter);
  }

  /* File will be opened only if there are .entry symbols */
  if (NULL != entry_file) {
    fclose(entry_file);
  }
  free (str_to_write);
  return SUCCESS;
}


static result_t GenerateExternFile(symbol_table_t *symbol_table,
                                   char *output_path,
                                   ext_symbol_occurrences_t *external_symbol_data_list) {
  FILE *extern_file = NULL;
  char *str_to_write = NULL;
  node_t *iter = GetHead(external_symbol_data_list->external_symbols);
  external_symbol_data_t *external_symbol = NULL;
  int i = 0;

  /* No external symbols, and thus no need to write a file */
  if (NULL == iter) {
    return SUCCESS; 
  }

  /* Allocate resources */
  str_to_write = (char *) malloc (MAX_LINE_LENGTH*sizeof(char));
  if (NULL == str_to_write) {
    fprintf(stderr, "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;   
  }

  extern_file = fopen(output_path, "a");
  if (NULL == extern_file) {
    free (str_to_write);
    perror("Couldn't open extern file");
    return ERROR_OPENING_FILE; 
  }

  while (NULL != iter) {
    external_symbol = GetValue(iter);

    for (i = 0; i < GetCapacityVector(external_symbol->occurrences); i++) {
      sprintf(str_to_write, "%s %04d\n",
              external_symbol->symbol_name,
              *((unsigned int *)GetElementVector(external_symbol->occurrences,i)));

      if (EOF == fputs(str_to_write, extern_file)) {
        free (str_to_write);
        fclose (extern_file);
        perror("Error writing to file");
        return ERROR_WRITING_TO_FILE;
      }
    }
    iter = GetNext(iter);
  }

  free (str_to_write);
  fclose(extern_file);
  return SUCCESS;
}

static result_t WriteHeader (char *output_path, int IC, int DC){
  FILE *obj_file;
  char *str_to_write = NULL;  

  str_to_write = (char *) malloc (MAX_LINE_LENGTH*sizeof(char));
  if (NULL == str_to_write){
      fprintf(stderr, "Memory allocation error: couldn't allocate a buffer\n");
      return MEM_ALLOCATION_ERROR;   
  }
  sprintf(str_to_write, "%d %d\n", IC, DC);
  obj_file = fopen(output_path, "w");
  if (obj_file == NULL) {
      free (str_to_write);
      return ERROR_OPENING_FILE;
  }

  if (fputs(str_to_write, obj_file) == EOF) {
      free (str_to_write);
      fclose(obj_file);
      return ERROR_WRITING_TO_FILE;
  }
  free (str_to_write);
  fclose(obj_file);

  return SUCCESS;
}

static int ExternalSymbolCompare (void *value, void *key) {
  char *symbol_name = (char *)key;
  external_symbol_data_t *external_symbol_data = (external_symbol_data_t *)value;

  return (0 == strcmp(external_symbol_data->symbol_name, symbol_name));
}
