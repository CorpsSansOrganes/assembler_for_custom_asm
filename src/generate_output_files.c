
#include "utils.h"
#include "bitmap.h"
#include "vector.h"
#include "symbol_table.h"
#include "preprocessing.h"
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#define ADDRESS_LENGTH 4
#define BITMAP_LENGTH 5

result_t GenerateOBJFile (vector_t *opcode, char *output_path, int IC, int DC){
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
            perror("Error writing to file");
            return ERROR_WRITING_TO_FILE;
        }
    }
    symbol = GetNextSymbol (symbol);
  }
  return SUCCESS;
}


static result_t GenerateExternFile (symbol_table_t *symbol_table, char *input_path, char *output_path){
  int line_counter=0;
  FILE *input_file = NULL;
  FILE *extern_file = NULL;
  char *line =NULL;
  const char *extern_word = ".extern ";
  char *search;
  char *str_to_write [40];/*character limit plus arbitrary number. TODO change*/
  line = (char *)malloc(MAX_LINE_SIZE * sizeof(char));
  while (NULL != fgets(line, MAX_LINE_SIZE, input_file)) {
        line_counter++;
        search = strstr (line,extern_word);
        if (NULL == search){
            str_to_write[0]= '\0';
            search += strlen(extern_word);
            search = strtok (search, blank_delimiters);
            extern_file = fopen(output_path, "a");
            if (NULL == extern_file) {
                perror("Couldn't open extern file");
                return ERROR_OPENING_FILE; 
            }
            break;
        } 
  }
    while (NULL != fgets(line, MAX_LINE_SIZE, input_file)) {
        if (NULL != search){
            str_to_write[0]= '\0';
            search += strlen(extern_word);
            search = strtok (search, blank_delimiters);
            extern_file = fopen(output_path, "a");
        } 
        line_counter++;
        search = strstr (line,extern_word);
  }
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

/*maximum 5 digits*/
void IntToConstDigitString(int num, char *output, const int num_of_digits) {
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