
#include "utils.h"
#include "bitmap.h"
#include "vector.h"
#include "symbol_table.h"
#include "preprocessing.h"
#include "string_utils.h"
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#define ADDRESS_LENGTH 4
#define BITMAP_LENGTH 5

static void IntToConstDigitString(int num, char *output, const int num_of_digits);
static result_t WriteHeader (char *output_path, int IC, int DC);
static result_t GenerateExternFile (symbol_table_t *symbol_table, char *output_path);
static result_t GenerateEntriesFile (symbol_table_t *symbol_table, char *output_path);
static result_t GenerateOBJFile (vector_t *opcode, char *output_path, int IC, int DC);


result_t GenerateOutputFiles (vector_t *opcode, symbol_table_t *symbol_table, char *input_path, int IC, int DC){
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
    if (SUCCESS != GenerateExternFile(symbol_table, extern_path)){
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
    char *str_to_write [11] = "";/*ADDRESS LENGTH + " " +BITMAP LENGTH +terminating null*/
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
            IntToConstDigitString (counter,address,ADDRESS_LENGTH);
            single_opcode = GetElementVector (opcode_line,j);
            IntToConstDigitString (single_opcode,bitmap,BITMAP_LENGTH);
            sprintf(str_to_write, "%s %s\n", counter, address);
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
  char *str_to_write [39] = "";/*character limit + len (max lines in code) + " \n" + terminating null*/
  symbol_t *symbol = GetHeadSymbol (symbol_table);
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
    symbol = GetNextSymbol (symbol);
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
    symbol = GetNextSymbol (symbol);
  }
  fclose(entry_file);
  return SUCCESS;
}


static result_t GenerateExternFile (symbol_table_t *symbol_table, char *output_path){
  FILE *extern_file = NULL;
  char *str_to_write [39] = "";/*character limit + len (max lines in code) + " \n" + terminating null*/
  symbol_t *symbol = GetHeadSymbol (symbol_table);
  vector_t *line_numbers;
  int i;
  if (NULL == symbol){
    return SUCCESS; /*no need to make file*/
  }
  while (NULL != symbol){
    if (EXTERN == GetSymbolType(symbol) && (NULL != line_number_vector)){/*pseudocode. to change*/
        extern_file = fopen(output_path, "a");
        if (NULL == extern_file) {
            free (extern_file);
            perror("Couldn't open extern file");
            return ERROR_OPENING_FILE; 
        }
        break;
    }
    symbol = GetNextSymbol (symbol);
  }
    while (NULL != symbol){
    if (EXTERN == GetSymbolType(symbol)){
        line_numbers = getsymbolLineNumbers (symbol);/*pseudocode, to change*/
        for (i=0; i<GetSizeVector(line_numbers);i++){
            sprintf (str_to_write, "%s %d\n", GetSymbolName(symbol),GetElementVector(line_numbers,i));
            if (EOF == fputs(str_to_write, extern_file)) {
                perror("Error writing to file");
                return ERROR_WRITING_TO_FILE;
            }
        }
    }
    symbol = GetNextSymbol (symbol);
  }
  fclose(extern_file);
  return SUCCESS;
  }


static result_t WriteHeader (char *output_path, int IC, int DC){
    FILE *obj_file;
    char str_to_write[15];  
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

static int externalSymbolCompare (char *symbol_name, char *key) {
  return (0 == strcmp (symbol_name, key));
}
/*maximum 5 digits*/
static void IntToConstDigitString(int num, char *output, const int num_of_digits) {
    char temp [6];  
    int num_length;
    int leading_zeros;  
    
    sprintf(temp, "%d", num);
    num_length = strlen(temp);
    leading_zeros = num_of_digits - num_length;

    for (int i = 0; i < leading_zeros; i++) {
        output[i] = '0';
    }
    strcpy(output + leading_zeros, temp);
}