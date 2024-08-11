#include <stdio.h> /* printf */
#include <string.h> /* strcmp */
#include "test_utils.h"
#include "syntax_errors.h"
#include "symbol_table.h"
#include "macro_table.h"

/* This config is used for calling syntax checks silently */

test_info_t DetectExtraCharactersTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("DetectExtraCharacters");
  const char *no_extra_characters = "    ";
  const char *no_characters_at_all = "";
  const char *extra_characters = "abc  a";
  const char *blank_and_then_extra_characters = " a";

  if (TRUE == DetectExtraCharacters(no_extra_characters, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == DetectExtraCharacters(no_characters_at_all, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == DetectExtraCharacters(extra_characters, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == DetectExtraCharacters(blank_and_then_extra_characters, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}

test_info_t IsReservedNameTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("IsReservedName");
  const char *reserved_directive = ".entry";
  const char *reserved_instruction = "cmp";
  const char *register_name = "r3";
  const char *not_reserved = "aaaaa";

  if (TRUE == IsReservedName(reserved_directive, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == IsReservedName(reserved_instruction, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == IsReservedName(register_name, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == IsReservedName(not_reserved, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}

/*
test_info_t InstructionDoesntExistTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("InstructionDoesntExist");
  const char *reserved_instruction = "cmp";
  const char *reserved_directive = ".entry";
  const char *not_reserved = "aaaaa";

  if (TRUE == InstructionDoesntExist(reserved_instruction, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == InstructionDoesntExist(reserved_directive, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == InstructionDoesntExist(not_reserved, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t WrongNumberOfOperandsTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("WrongNumberOfOperands");
  const char *cmp_instruction = "cmp";
  const int cmp_operands = 2;
  const int not_cmp_operands = 0;
  const char *stop_instruction = "cmp";
  const int not_stop_operands = 1;
  const int stop_operands = 0;
  const int expected_result_stop = stop_operands-not_stop_operands;
  const int expected_result_cmp = cmp_operands-not_cmp_operands;
  if (FALSE != WrongNumberOfOperands(cmp_instruction,cmp_operands, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
 if (FALSE != WrongNumberOfOperands(stop_instruction,stop_operands, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (expected_result_cmp != WrongNumberOfOperands(cmp_instruction,not_cmp_operands, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (expected_result_stop != WrongNumberOfOperands(stop_instruction,not_stop_operands, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t IncorrectAddressingMethodTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("IncorrectAddressingMethod");
  operand_type_t source = SOURCE_OPERAND;
  operand_type_t destination = DESTINATION_OPERAND;
  const char *add_instruction = "add";
  const char *jmp_instruction = "jmp";
  char invalid_operand1 = "#c";
  char invalid_operand2 = "#";
  char invalid_operand3 = "*r13";
  char invalid_operand4 = "#++12";
  char valid_operand0 = "#12";
  char valid_operand1 = "aaaa";
  char valid_operand2 = "*r2";
  char valid_operand3 = "r4";
  if (FALSE == IncorrectAddressingMethod(add_instruction,invalid_operand1,destination, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE ==  IncorrectAddressingMethod(add_instruction,invalid_operand2,source, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == IncorrectAddressingMethod(jmp_instruction,invalid_operand3,destination, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE ==  IncorrectAddressingMethod(jmp_instruction,invalid_operand4,source, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE ==  IncorrectAddressingMethod(jmp_instruction,valid_operand1,source, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE ==  IncorrectAddressingMethod(add_instruction,valid_operand0,destination, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE ==  IncorrectAddressingMethod(jmp_instruction,valid_operand2,destination, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE ==  IncorrectAddressingMethod(add_instruction,valid_operand3,source, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolDefinedMoreThanOnceTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolDefinedMoreThanOnce");
  symbol_table_t *test_table = CreateSymbolTable();
  result_t *valid_test_symbol = AddSymbol(test_table,"aaaa",100); 
  if (FALSE == SymbolDefinedMoreThanOnce("aaaa",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolDefinedMoreThanOnce("bbbb",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t ColonSyntaxErrorTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("ColonSyntaxError");
  const char *no_colon = "aaaa";
  const char *colon_with_space = "aaaa :";
  const char *valid = "aaaa:";

  if (TRUE == ColonSyntaxError(no_colon, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == ColonSyntaxError(colon_with_space, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == ColonSyntaxError(valid, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolWasntDefinedTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolDefinedMoreThanOnce");
  symbol_table_t *test_table = CreateSymbolTable();
  AddSymbol(test_table,"aaaa",100); 
  if (TRUE == SymbolWasntDefined("aaaa",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolWasntDefined("bbbb",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolAlreadyDefinedAsEntryTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolAlreadyDefinedAsEntry");
  symbol_table_t *test_table = CreateSymbolTable();
  AddExternalSymbol(test_table,"aaaa",100); 
  AddEntrySymbol(test_table,"bbbb",100); 
  AddSymbol(test_table,"cccc",100); 
  if (TRUE == SymbolAlreadyDefinedAsEntry("aaaa",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolAlreadyDefinedAsEntry("cccc",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolAlreadyDefinedAsEntry("bbbb",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolAlreadyDefinedAsExternTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolAlreadyDefinedAsExtern");
  symbol_table_t *test_table = CreateSymbolTable();
  AddExternalSymbol(test_table,"aaaa",100); 
  AddEntrySymbol(test_table,"bbbb",100); 
  AddSymbol(test_table,"cccc",100); 
  if (FALSE == SymbolAlreadyDefinedAsExtern("aaaa",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolAlreadyDefinedAsExtern("cccc",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolAlreadyDefinedAsExtern("bbbb",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolIsIllegalTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolIsIllegal");
  const char *space_in_between = "asc da";
  const char *invalid_symbol = "?-c?";
  const char *valid_symbol = "ascasv";

  if (FALSE == SymbolIsIllegal(space_in_between, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolIsIllegal(invalid_symbol, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolIsIllegal(valid_symbol, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolPrefixIllegalTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolPrefixIllegal");
  const char *invalid_sprefix1 = "1da";
  const char *invalid_sprefix2 = " ?-c?";
  const char *valid_prefix = "aaaa";

  if (FALSE == SymbolPrefixIllegal(invalid_sprefix1, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolPrefixIllegal(invalid_sprefix2, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolPrefixIllegal(valid_prefix, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolExceedCharacterLimitTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolExceedCharacterLimit");
  const char *exceed = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  const char *not_exceed = "aaaa";

  if (FALSE == SymbolExceedCharacterLimit(exceed, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolExceedCharacterLimit(not_exceed, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
*/
/*
*to do
*/
/*
test_info_t SymbolUsedAsAMacroTest(syntax_check_config_t *cfg){
      test_info_t test_info = InitTestInfo("SymbolUsedAsAMacroTest");
  macro_table_t *test_table = CreateMacroTable();
  AddMacroIfUnique(test_table,"aaaa","definition"); 
  if (FALSE ==  SymbolUsedAsAMacro("aaaa",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE ==  SymbolUsedAsAMacro("bbbb",test_table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}


test_info_t DirectiveDoesntExistTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("DirectiveDoesntExist");
  const char *exist = ".entry";
  const char *without_dot = "entry";
  const char *not_exist = "aaaa";
  if (TRUE == DirectiveDoesntExist(exist, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == DirectiveDoesntExist(without_dot, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
    if (FALSE == DirectiveDoesntExist(not_exist, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t DirectiveIsUpperCaseTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("DirectiveIsUpperCase");
  const char *exist = ".eNTry";
  const char *without_dot = ".entry";
  const char *not_exist = "aaaa";
  if (FALSE == DirectiveIsUpperCase(exist, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == DirectiveIsUpperCase(without_dot, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
    if (TRUE == DirectiveIsUpperCase(not_exist, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t CommaIsMissingInDataTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("CommaIsMissingInData");
  const char *valid = "aaa, bbb, ccc";
  const char *with_space = "aaaa , bbbb";
  const char *no_comma = "aaaa bbbb";
  if (FALSE == CommaIsMissingInData(no_comma, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == CommaIsMissingInData(with_space, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
    if (TRUE == CommaIsMissingInData(valid, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t RegisterNameDoesntExistTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("RegisterNameDoesntExist");
  const char *valid = "r0";
  const char *not_exist = "r9";
  const char *not_a_register = "aaaabbbb";
  if (FALSE == RegisterNameDoesntExist(valid, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == RegisterNameDoesntExist(not_exist, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
    if (TRUE == RegisterNameDoesntExist(not_a_register, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
*/

int main(int argc, char *argv[]) {
  int total_failures = 0;
  syntax_check_config_t cfg;
  test_info_t test_info;
  bool_t verbose = FALSE;
  int i = 0;

  for (i = 0; i < argc; ++i) {
    if (0 == strcmp(argv[i], "-v")) {
      verbose = TRUE;
    }
  }

  cfg = CreateSyntaxCheckConfig(argv[0], -1, verbose);

  test_info = DetectExtraCharactersTest(&cfg);
  if (TEST_SUCCESSFUL != test_info.result) {
    PrintTestInfo(test_info);
    ++total_failures;
  }

  test_info = IsReservedNameTest(&cfg);
  if (TEST_SUCCESSFUL != test_info.result) {
    PrintTestInfo(test_info);
    ++total_failures;
  }

  if (0 == total_failures) {
    printf("Test successful: Syntax errors\n");
  }
  return total_failures;
}
