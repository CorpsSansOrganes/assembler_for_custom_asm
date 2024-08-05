#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "test_utils.h"

test_info_t CreateSymbolTableTest(void) {
  /* 
   * This is a smoke test.
   * It may also be used w/ valgrind to check for memory leaks.
  */ 

  test_info_t test_info = InitTestInfo("CreateSymbolTable");

  symbol_table_t *table = CreateSymbolTable();
  if(NULL == table) {
    RETURN_ERROR(TEST_FAILED);
  }

  DestroySymbolTable(table);
  return test_info;
}

test_info_t AddSymbolTest(void) {
  test_info_t test_info = InitTestInfo("AddSymbol");
  int n = 4;
  int i = 0;
  const char *symbols[] = {"a", "b", "c", "a"};
  symbol_table_t *table = CreateSymbolTable();
  address_t address = 100;

  if(NULL == table) {
    RETURN_ERROR(TECHNICAL_ERROR);
  }
  
  for (i = 0; i < n; ++i) {
    AddSymbol(table, symbols[i], address);
    ++address;
  }

  /* Use valgrind to make sure all memory allocated is freed! */
  DestroySymbolTable(table);
  return test_info;
}

test_info_t FindSymbolTest(void) {
  test_info_t test_info = InitTestInfo("AddSymbol");
  int n = 4;
  int i = 0;
  const char *symbols[] = {"a", "b", "c", "a"};
  symbol_table_t *table = CreateSymbolTable();
  address_t address = 100;
  address_t *result = (address_t *)malloc(sizeof(address_t));

  if (NULL == result) {
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if(NULL == table) {
    free(result);
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (FAILURE != FindSymbol(table, "empty_table", result)) {
    DestroySymbolTable(table);
    free(result);
    RETURN_ERROR(TEST_FAILED);
  }
  
  for (i = 0; i < n; ++i) {
    AddSymbol(table, symbols[i], address);
    ++address;
  }

  if (SUCCESS != FindSymbol(table, "b", result)) {
    DestroySymbolTable(table);
    free(result);
    RETURN_ERROR(TEST_FAILED);
  }

  if (101 != *result) {
    DestroySymbolTable(table);
    free(result);
    RETURN_ERROR(TEST_FAILED);
  }

  if (SUCCESS != FindSymbol(table, "a", result)) {
    DestroySymbolTable(table);
    free(result);
    RETURN_ERROR(TEST_FAILED);
  }

  if (100 != *result) {
    DestroySymbolTable(table);
    free(result);
    RETURN_ERROR(TEST_FAILED);
  }

  if (FAILURE != FindSymbol(table, "x", result)) {
    DestroySymbolTable(table);
    free(result);
    RETURN_ERROR(TEST_FAILED);
  }

  if (SYMBOL_NOT_FOUND != *result) {
    DestroySymbolTable(table);
    free(result);
    RETURN_ERROR(TEST_FAILED);
  }

  DestroySymbolTable(table);
  free(result);
  return test_info;
}

int main(void) {
  int total_failures = 0;
  test_info_t test_info;

  test_info = CreateSymbolTableTest();
  if (TEST_SUCCESSFUL != test_info.result) {
    PrintTestInfo(test_info);
    ++total_failures;
  }

  test_info = AddSymbolTest();
  if (TEST_SUCCESSFUL != test_info.result) {
    PrintTestInfo(test_info);
    ++total_failures;
  }

  test_info = FindSymbolTest();
  if (TEST_SUCCESSFUL != test_info.result) {
    PrintTestInfo(test_info);
    ++total_failures;
  }

  if (0 == total_failures) {
    printf("Test successful: symbol table\n");
  }

  return total_failures;
}
