
#include "utils.h"
#include "bitmap.h"
#include "vector.h"
#include "symbol_table.h"
#include "assembler.h"
#include "preprocessing.h"
#include "string_utils.h"
#include "language_definitions.h"
#include "generate_output_files.h"
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>


static result_t WriteHeader (char *output_path, int IC, int DC);
static result_t GenerateEntriesFile (symbol_table_t *symbol_table, char *output_path);
static result_t GenerateOBJFile (vector_t *code_opcode, vector_t *data_opcode, char *output_path);
static result_t GenerateExternFile (symbol_table_t *symbol_table, char *output_path,ext_symbol_occurences_t *external_symbol_data_list);
static int ExternalSymbolCompare (void *value, void *key);


ext_symbol_occurences_t *CreateExternalSymbolList(){
    ext_symbol_occurences_t external_symbol_list;
    list_t *list = CreateList ();
    if (NULL == list){
        perror ("error creating file");
        return NULL;
    }
    external_symbol_list.external_symbols = list;
}
result_t AddExternalSymbolOccurence(ext_symbol_occurences_t *ext_symbol_occurences, const char *symbol_name, unsigned int line){
    external_symbol_data_t *external_symbol_data;
    node_t *node = Find(ext_symbol_occurences->external_symbols,
                        ExternalSymbolCompare,
                             symbol_name);
         /* if thats the first occurence */
    if (NULL == node) {
        external_symbol_data = malloc (sizeof(external_symbol_data_t *));
        external_symbol_data->symbol_name = StrDup (symbol_name);
        if (NULL == external_symbol_data->symbol_name){
            perror ("error duplicating name");
            return FAILURE;
        }
        external_symbol_data->occurences = CreateVector (3,sizeof(int));
        if (NULL == external_symbol_data->occurences){
            perror ("error creating vector");
            return FAILURE;
        }
        AddNode (ext_symbol_occurences->external_symbols,external_symbol_data);
        if (NULL == AddNode (ext_symbol_occurences->external_symbols,external_symbol_data)){
            perror ("error creating vector");
            return FAILURE;
        }
    }
    else {
        external_symbol_data = GetValue(node);
    }
    if (SUCCESS != AppendVector (external_symbol_data->occurences,line)){
        perror ("error appending vector");
        return FAILURE;
    }
    return SUCCESS;

}
void DestroyExternSymbolList(ext_symbol_occurences_t *ext_symbol_occurences){
    external_symbol_data_t *external_symbol_data;
    node_t *node = GetHead(ext_symbol_occurences->external_symbols);
    while (NULL != node)
    {
        external_symbol_data = GetValue(node);
        DestroyVector(external_symbol_data->occurences);
        free (external_symbol_data ->symbol_name);
        free (external_symbol_data);
        node = GetNext(node);
    }
    return;
}


result_t GenerateOutputFiles (vector_t *code_table,
                              vector_t *data_table,
                              symbol_table_t *symbol_table,
                              char *input_path,
                              ext_symbol_occurences_t* ext_symbol_occurrences){
    char *obj_path = StrDup(input_path);
    char *extern_path = StrDup(input_path);
    char *entry_path = StrDup(input_path);
    /*add the correct finish*/
    strcpy(obj_path + strlen(input_path), ".ob");
    strcpy(entry_path + strlen(input_path), ".ent");
    strcpy(extern_path + strlen(input_path), ".ext");
    if (SUCCESS != GenerateOBJFile(code_table,data_table, obj_path)){
        free (obj_path);
        free (extern_path);
        free (entry_path);
        return FAILURE;
    }
    if (SUCCESS != GenerateExternFile(symbol_table, extern_path,ext_symbol_occurrences)){
        free (obj_path);
        free (extern_path);
        free (entry_path);        
        return FAILURE;
    }
    DestroyExternSymbolList (ext_symbol_occurrences);
    if (SUCCESS != GenerateEntriesFile(symbol_table,entry_path)){
        free (obj_path);
        free (extern_path);
        free (entry_path);
        return FAILURE;
    }
    free (obj_path);
    free (extern_path);
    free (entry_path);
    return SUCCESS;
}

static result_t GenerateOBJFile (vector_t *code_opcode, vector_t *data_opcode, char *output_path){
    int counter = 100;
    vector_t *opcode_line;
    bitmap_t single_opcode;
    int i;
    char *address = (char *) malloc (6*sizeof(char));
    char *bitmap =  (char *) malloc (6*sizeof(char));
    char *str_to_write = (char *) malloc (MAX_LINE_LENGTH*sizeof(char));
    FILE *obj_file = NULL;
    if (SUCCESS != WriteHeader (output_path, GetSizeVector(code_opcode), GetSizeVector(data_opcode))){
        free (address);
        free (bitmap);
        free (str_to_write);
        return FAILURE;
    }
    obj_file = fopen(output_path, "a");
    if (NULL == obj_file) {
        free (address);
        free (bitmap);
        free (str_to_write);
        perror("Couldn't open obj file");
        return ERROR_OPENING_FILE; 
    }
    for (i=0; i<GetSizeVector(code_opcode);i++) {
        opcode_line = GetElementVector(code_opcode,i);
        sprintf(address, "%04d", counter);
        sprintf(bitmap, "%05o", single_opcode);
        sprintf(str_to_write, "%s %s\n", address, bitmap);
        if (EOF == fputs(str_to_write, obj_file)) {
            perror("Error writing to file");
            fclose (obj_file);
            free (address);
            free (bitmap);
            free (str_to_write);
            return ERROR_WRITING_TO_FILE;
        }
        counter++;
        
    }
    for (i=0; i<GetSizeVector(data_opcode);i++) {
        opcode_line = GetElementVector(data_opcode,i);
        sprintf(address, "%04d", counter);
        sprintf(bitmap, "%05o", single_opcode);
        sprintf(str_to_write, "%s %s\n", address, bitmap);
        if (EOF == fputs(str_to_write, obj_file)) {
            perror("Error writing to file");
            fclose (obj_file);
            free (address);
            free (bitmap);
            free (str_to_write);
            return ERROR_WRITING_TO_FILE;
        }
        counter++;
    }    
    free (address);
    free (bitmap);
    free (str_to_write);
    fclose(obj_file);
    return SUCCESS;
}


static result_t GenerateEntriesFile (symbol_table_t *symbol_table, char *output_path){
  FILE *entry_file = NULL;
  list_t *symbol_list =AsList(symbol_table);
  char *str_to_write = NULL;
  symbol_t *symbol = (symbol_t *) GetHead (symbol_list);

  str_to_write = (char *) malloc (MAX_LINE_LENGTH*sizeof(char));
  if (NULL == str_to_write){
    fprintf(stderr, "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;   
  }
  if (NULL == symbol){
    free (str_to_write);
    return SUCCESS; /*no need to make file*/
  }
  while (NULL != symbol){
    if (ENTRY == GetSymbolType(symbol)){
        entry_file = fopen(output_path, "a");
        if (NULL == entry_file) {
            free (str_to_write);
            perror("Couldn't open entry file");
            return ERROR_OPENING_FILE; 
        }
        break;
    }
    symbol = (symbol_t *) GetNext(symbol);
  }
    while (NULL != symbol){
    if (ENTRY == GetSymbolType(symbol)){
        sprintf (str_to_write, "%s %d\n", GetSymbolName(symbol),GetSymbolAddress(symbol));
        if (EOF == fputs(str_to_write, entry_file)) {
            fclose (entry_file);
            free (str_to_write);
            perror("Error writing to file");
            return ERROR_WRITING_TO_FILE;
        }
    }
    symbol = (symbol_t *) GetNext (symbol);
  }
  fclose(entry_file);
  free (str_to_write);
  return SUCCESS;
}


static result_t GenerateExternFile (symbol_table_t *symbol_table, char *output_path,ext_symbol_occurences_t *external_symbol_data_list){
  FILE *extern_file = NULL;
  char *str_to_write = NULL;
  external_symbol_data_t *external_symbol;
  external_symbol =  GetHead (external_symbol_data_list);
  int i;

  str_to_write = (char *) malloc (MAX_LINE_LENGTH*sizeof(char));
  if (NULL == str_to_write){
    fprintf(stderr, "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;   
  }
  if (NULL == external_symbol){
    free (str_to_write);
    return SUCCESS; /*no need to make file*/
  }
  extern_file = fopen(output_path, "a");
  if (NULL == extern_file) {
    free (str_to_write);
    perror("Couldn't open extern file");
    return ERROR_OPENING_FILE; 
  }
  while (NULL != external_symbol){
        for (i=0; i<GetCapacityVector(external_symbol->occurences);i++){
            sprintf (str_to_write, "%s %04d\n", external_symbol, GetElementVector(external_symbol->occurences,i));
        }
         if (EOF == fputs(str_to_write, extern_file)) {
                free (str_to_write);
                fclose (extern_file);
                perror("Error writing to file");
                return ERROR_WRITING_TO_FILE;
        }
    external_symbol = (external_symbol_data_t *) GetNext (external_symbol);
  }
  free (str_to_write);
  fclose(extern_file);
  return SUCCESS;
  }


static result_t WriteHeader (char *output_path, int IC, int DC){
    FILE *obj_file;
    char str_to_write = NULL;  

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
}

static int ExternalSymbolCompare (void *value, void *key) {
  char *symbol_name = (char *)key;
  external_symbol_data_t *external_symbol_data = (external_symbol_data_t *)value;

  return (0 == strcmp(external_symbol_data->symbol_name, key));
}