
#include "utils.h"
#include "bitmap.h"
#include "vector.h"
#include "symbol_table.h"
#include "assembler.h"
#include "preprocessing.h"
#include "string_utils.h"
#include "language_definitions.h"
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#define ADDRESS_LENGTH 4
#define BITMAP_LENGTH 5

static result_t WriteHeader (char *output_path, int IC, int DC);
static result_t GenerateExternFile (symbol_table_t *symbol_table, char *output_path);
static result_t GenerateEntriesFile (symbol_table_t *symbol_table, char *output_path);
static result_t GenerateOBJFile (vector_t *opcode, char *output_path, int IC, int DC);
static result_t GenerateExternFile (symbol_table_t *symbol_table, char *output_path,list_t *external_symbol_data_list);


result_t GenerateOutputFiles (vector_t *opcode, symbol_table_t *symbol_table, char *input_path, list_t *external_symbol_data_list, int IC, int DC){
    char *obj_path = StrDup(input_path);
    char *extern_path = StrDup(input_path);
    char *entry_path = StrDup(input_path);
    /*replace .am with the correct finish*/
    strcpy(obj_path + strlen(input_path) - 3, ".ob");
    strcpy(entry_path + strlen(input_path) - 3, ".ent");
    strcpy(extern_path + strlen(input_path) - 3, ".ext");
    if (SUCCESS != GenerateOBJFile(opcode,obj_path,IC,DC)){
        return FAILURE;
    }
    if (SUCCESS != GenerateExternFile(symbol_table, extern_path,external_symbol_data_list)){
        return FAILURE;
    }
    if (SUCCESS != GenerateEntriesFile(symbol_table,entry_path)){
        return FAILURE;
    }
    return SUCCESS;
}

static result_t GenerateOBJFile (vector_t *opcode, char *output_path, int IC, int DC){
    int counter = 100;
    vector_t *opcode_line;
    bitmap_t single_opcode;
    int i;
    int j;
    char *address [6];
    char *bitmap [6];
    FILE *obj_file = NULL;
    char *str_to_write [MAX_LINE_LENGTH] = "";
    if (SUCCESS != WriteHeader (output_path, IC, DC)){
        return FAILURE;
    }
    obj_file = fopen(output_path, "a");
    if (NULL == obj_file) {
        perror("Couldn't open obj file");
        return ERROR_OPENING_FILE; 
    }
    for (i=0; i<GetSizeVector(opcode);i++)
    {
        opcode_line = GetElementVector(opcode,i);
        for (j=0; j<GetSizeVector(opcode_line); j++)
        {
            str_to_write[0] = "\0";
            sprintf(address, "%04d", counter);
            single_opcode = GetElementVector (opcode_line,j);
            sprintf(bitmap, "%05o", single_opcode);
            sprintf(str_to_write, "%s %s\n", address, bitmap);
            if (EOF == fputs(str_to_write, obj_file)) {
                perror("Error writing to file");
                free (obj_file);
                return ERROR_WRITING_TO_FILE;
            }
            counter++;

        }
        
    }
    fclose(obj_file);
    return SUCCESS;
}


static result_t GenerateEntriesFile (symbol_table_t *symbol_table, char *output_path){
  FILE *entry_file = NULL;
  list_t *symbol_list =AsList(symbol_table);
  char *str_to_write [MAX_LINE_LENGTH] = "";
  symbol_t *symbol = (symbol_t *) GetHead (symbol_list);
  if (NULL == symbol){
    return SUCCESS; /*no need to make file*/
  }
  while (NULL != symbol){
    if (ENTRY == GetSymbolType(symbol)){
        entry_file = fopen(output_path, "a");
        if (NULL == entry_file) {
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
            free (entry_file);
            perror("Error writing to file");
            return ERROR_WRITING_TO_FILE;
        }
    }
    symbol = (symbol_t *) GetNext (symbol);
  }
  fclose(entry_file);
  return SUCCESS;
}


static result_t GenerateExternFile (symbol_table_t *symbol_table, char *output_path,list_t *external_symbol_data_list){
  FILE *extern_file = NULL;
  char *str_to_write [MAX_LINE_LENGTH] = "";
  external_symbol_data_t *external_symbol;
  external_symbol =  GetHead (external_symbol_data_list);
  int i;
  if (NULL == external_symbol){
    return SUCCESS; /*no need to make file*/
  }
  while (NULL != external_symbol){
        for (i=0; i<GetCapacityVector(external_symbol->occurences);i++){
            sprintf (str_to_write, "%s %d\n", external_symbol, GetElementVector(external_symbol->occurences,i));
        }
         if (EOF == fputs(str_to_write, extern_file)) {
                perror("Error writing to file");
                return ERROR_WRITING_TO_FILE;
        }
    external_symbol = (external_symbol_data_t *) GetNext (external_symbol);
  }
  fclose(extern_file);
  return SUCCESS;
  }


static result_t WriteHeader (char *output_path, int IC, int DC){
    FILE *obj_file;
    char str_to_write[MAX_LINE_LENGTH];  
    int total_chars;


    total_chars = sprintf(str_to_write, "%d %d\n", IC, DC);

    if (total_chars < 0 || total_chars >= sizeof(str_to_write)) {
        return FAILURE;
    }

    obj_file = fopen(output_path, "w");
    if (obj_file == NULL) {
        return ERROR_OPENING_FILE;
    }

    if (fputs(str_to_write, obj_file) == EOF) {
        return ERROR_WRITING_TO_FILE;
    }

    fclose(obj_file);
}