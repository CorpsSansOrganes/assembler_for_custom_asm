#include "test_utils.h"
#include "syntax_errors.h"
#include "symbol_table.h"
#include "macro_table.h"

error_check_info_t error_check_default = {FALSE, 0, "test"};

test_info_t DetectExtraCharactersTest(void) {
  test_info_t test_info = InitTestInfo("DetectExtraCharacters");
  const char *no_extra_characters = "    ";
  const char *no_characters_at_all = "";
  const char *extra_characters = "abc  a";
  const char *blank_and_then_extra_characters = " a";

  if (TRUE == DetectExtraCharacters(no_extra_characters, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == DetectExtraCharacters(no_characters_at_all, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == DetectExtraCharacters(no_extra_characters, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == DetectExtraCharacters(blank_and_then_extra_characters, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t IsReservedNameTest(void) {
  test_info_t test_info = InitTestInfo("IsReservedName");
  const char *reserved_directive = ".entry";
  const char *reserved_instruction = "cmp";
  const char *register_name = "r3";
  const char *not_reserved = "aaaaa";

  if (TRUE == IsReservedName(reserved_directive, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == IsReservedName(reserved_instruction, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == IsReservedName(register_name, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == IsReservedName(not_reserved, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}

test_info_t InstructionDoesntExistTest(void) {
  test_info_t test_info = InitTestInfo("InstructionDoesntExist");
  const char *reserved_instruction = "cmp";
  const char *reserved_directive = ".entry";
  const char *not_reserved = "aaaaa";

  if (TRUE == InstructionDoesntExist(reserved_instruction, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == InstructionDoesntExist(reserved_directive, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == InstructionDoesntExist(not_reserved, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t WrongNumberOfOperandsTest(void) {
  test_info_t test_info = InitTestInfo("WrongNumberOfOperands");
  const char *cmp_instruction = "cmp";
  const int cmp_operands = 2;
  const int not_cmp_operands = 0;
  const char *stop_instruction = "cmp";
  const int not_stop_operands = 1;
  const int stop_operands = 0;
  const int expected_result_stop = stop_operands-not_stop_operands;
  const int expected_result_cmp = cmp_operands-not_cmp_operands;
  if (FALSE != WrongNumberOfOperands(cmp_instruction,cmp_operands, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
 if (FALSE != WrongNumberOfOperands(stop_instruction,stop_operands, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (expected_result_cmp != WrongNumberOfOperands(cmp_instruction,not_cmp_operands, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (expected_result_stop != WrongNumberOfOperands(stop_instruction,not_stop_operands, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t IncorrectAddressingMethodTest(void) {
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
  if (FALSE == IncorrectAddressingMethod(add_instruction,invalid_operand1,destination)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE ==  IncorrectAddressingMethod(add_instruction,invalid_operand2,source, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == IncorrectAddressingMethod(jmp_instruction,invalid_operand3,destination, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE ==  IncorrectAddressingMethod(jmp_instruction,invalid_operand4,source, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE ==  IncorrectAddressingMethod(jmp_instruction,valid_operand1,source, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE ==  IncorrectAddressingMethod(add_instruction,valid_operand0,destination, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE ==  IncorrectAddressingMethod(jmp_instruction,valid_operand2,destination, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE ==  IncorrectAddressingMethod(add_instruction,valid_operand3,source, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolDefinedMoreThanOnceTest(void) {
  test_info_t test_info = InitTestInfo("SymbolDefinedMoreThanOnce");
  symbol_table_t *test_table = CreateSymbolTable();
  result_t *valid_test_symbol = AddSymbol(test_table,"aaaa",100); 
  if (FALSE == SymbolDefinedMoreThanOnce("aaaa",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolDefinedMoreThanOnce("bbbb",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t ColonSyntaxErrorTest(void) {
  test_info_t test_info = InitTestInfo("ColonSyntaxError");
  const char *no_colon = "aaaa";
  const char *colon_with_space = "aaaa :";
  const char *valid = "aaaa:";

  if (TRUE == ColonSyntaxError(no_colon, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == ColonSyntaxError(colon_with_space, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == ColonSyntaxError(valid, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolWasntDefinedTest(void) {
  test_info_t test_info = InitTestInfo("SymbolDefinedMoreThanOnce");
  symbol_table_t *test_table = CreateSymbolTable();
  AddSymbol(test_table,"aaaa",100); 
  if (TRUE == SymbolWasntDefined("aaaa",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolWasntDefined("bbbb",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolAlreadyDefinedAsEntryTest(void) {
  test_info_t test_info = InitTestInfo("SymbolAlreadyDefinedAsEntry");
  symbol_table_t *test_table = CreateSymbolTable();
  AddExternalSymbol(test_table,"aaaa",100); 
  AddEntrySymbol(test_table,"bbbb",100); 
  AddSymbol(test_table,"cccc",100); 
  if (TRUE == SymbolAlreadyDefinedAsEntry("aaaa",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolAlreadyDefinedAsEntry("cccc",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolAlreadyDefinedAsEntry("bbbb",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolAlreadyDefinedAsExternTest(void) {
  test_info_t test_info = InitTestInfo("SymbolAlreadyDefinedAsExtern");
  symbol_table_t *test_table = CreateSymbolTable();
  AddExternalSymbol(test_table,"aaaa",100); 
  AddEntrySymbol(test_table,"bbbb",100); 
  AddSymbol(test_table,"cccc",100); 
  if (FALSE == SymbolAlreadyDefinedAsExtern("aaaa",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolAlreadyDefinedAsExtern("cccc",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolAlreadyDefinedAsExtern("bbbb",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolIsIllegalTest(void) {
  test_info_t test_info = InitTestInfo("SymbolIsIllegal");
  const char *space_in_between = "asc da";
  const char *invalid_symbol = "?-c?";
  const char *valid_symbol = "ascasv";

  if (FALSE == SymbolIsIllegal(space_in_between, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolIsIllegal(invalid_symbol, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolIsIllegal(valid_symbol, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolPrefixIllegalTest(void) {
  test_info_t test_info = InitTestInfo("SymbolPrefixIllegal");
  const char *invalid_sprefix1 = "1da";
  const char *invalid_sprefix2 = " ?-c?";
  const char *valid_prefix = "aaaa";

  if (FALSE == SymbolPrefixIllegal(invalid_sprefix1, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolPrefixIllegal(invalid_sprefix2, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == SymbolPrefixIllegal(valid_prefix, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t SymbolExceedCharacterLimitTest(void) {
  test_info_t test_info = InitTestInfo("SymbolExceedCharacterLimit");
  const char *exceed = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  const char *not_exceed = "aaaa";

  if (FALSE == SymbolExceedCharacterLimit(exceed, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == SymbolExceedCharacterLimit(not_exceed, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
/*
*to do
*/
test_info_t SymbolUsedAsAMacroTest(void){
      test_info_t test_info = InitTestInfo("SymbolUsedAsAMacroTest");
  macro_table_t *test_table = CreateMacroTable();
  AddMacroIfUnique(test_table,"aaaa","definition"); 
  if (FALSE ==  SymbolUsedAsAMacro("aaaa",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE ==  SymbolUsedAsAMacro("bbbb",test_table, error_check_default)){
     RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}


test_info_t DirectiveDoesntExistTest(void) {
  test_info_t test_info = InitTestInfo("DirectiveDoesntExist");
  const char *exist = ".entry";
  const char *without_dot = "entry";
  const char *not_exist = "aaaa";
  if (TRUE == DirectiveDoesntExist(exist, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == DirectiveDoesntExist(without_dot, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
    if (FALSE == DirectiveDoesntExist(not_exist, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t DirectiveIsUpperCaseTest(void) {
  test_info_t test_info = InitTestInfo("DirectiveIsUpperCase");
  const char *exist = ".eNTry";
  const char *without_dot = ".entry";
  const char *not_exist = "aaaa";
  if (FALSE == DirectiveIsUpperCase(exist, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE == DirectiveIsUpperCase(without_dot, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
    if (TRUE == DirectiveIsUpperCase(not_exist, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t CommaIsMissingInDataTest(void) {
  test_info_t test_info = InitTestInfo("CommaIsMissingInData");
  const char *valid = "aaa, bbb, ccc";
  const char *with_space = "aaaa , bbbb";
  const char *no_comma = "aaaa bbbb";
  if (FALSE == CommaIsMissingInData(no_comma, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == CommaIsMissingInData(with_space, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
    if (TRUE == CommaIsMissingInData(valid, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
test_info_t RegisterNameDoesntExistTest(void) {
  test_info_t test_info = InitTestInfo("RegisterNameDoesntExist");
  const char *valid = "r0";
  const char *not_exist = "r9";
  const char *not_a_register = "aaaabbbb";
  if (FALSE == RegisterNameDoesntExist(valid, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE == RegisterNameDoesntExist(not_exist, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
    if (TRUE == RegisterNameDoesntExist(not_a_register, error_check_default)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}
