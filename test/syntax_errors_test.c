#include <stdio.h> /* printf */
#include <string.h> /* strcmp */
#include "test_utils.h"
#include "syntax_errors.h"
#include "symbol_table.h"
#include "macro_table.h"


typedef test_info_t (* test_func_t)(syntax_check_config_t *);

test_info_t DetectExtraCharactersTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("DetectExtraCharacters");
  const char *no_extra_characters = "    ";
  const char *no_characters_at_all = "";
  const char *extra_characters = "abc  a";
  const char *blank_and_then_extra_characters = " a";

  if (FALSE != DetectExtraCharacters(no_extra_characters, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != DetectExtraCharacters(no_characters_at_all, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != DetectExtraCharacters(extra_characters, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != DetectExtraCharacters(blank_and_then_extra_characters, cfg)) {
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

  if (TRUE != IsReservedName(reserved_directive, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != IsReservedName(reserved_instruction, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != IsReservedName(register_name, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IsReservedName(not_reserved, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}

test_info_t MacroDefinedMoreThanOnceTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("MacroDefinedMoreThanOnce");
  macro_table_t *table = CreateMacroTable();
  const char *macro_name = "aaa";
  const char *not_macro_name = "ccc";
  const char *macro_definition = "bbb";

  AddMacro(table, macro_name, macro_definition);
  if (TRUE != MacroDefinedMoreThanOnce(macro_name, table, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (FALSE != MacroDefinedMoreThanOnce(not_macro_name, table, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

test_info_t InstructionDoesntExistTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("InstructionDoesntExist");
  const char *reserved_instruction = "cmp";
  const char *reserved_directive = ".entry";
  const char *not_reserved = "aaaaa";

  if (FALSE != InstructionDoesntExist(reserved_instruction, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != InstructionDoesntExist(reserved_directive, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != InstructionDoesntExist(not_reserved, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}

test_info_t WrongNumberOfOperandsTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("WrongNumberOfOperands");
  const char *cmp_instruction = "cmp";
  const int cmp_operands = 2;
  const int not_cmp_operands = 0;
  const char *stop_instruction = "stop";
  const int not_stop_operands = 1;
  const int stop_operands = 0;

  if (FALSE != WrongNumberOfOperands(cmp_instruction,cmp_operands, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (FALSE != WrongNumberOfOperands(stop_instruction,stop_operands, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* Too little */
  if (TRUE != WrongNumberOfOperands(cmp_instruction,not_cmp_operands, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* Too much */
  if (TRUE != WrongNumberOfOperands(stop_instruction,not_stop_operands, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}

test_info_t IncorrectAddressingMethodTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("IncorrectAddressingMethod");

  operand_t immediate_source_operand = {"#2", IMMEDIATE, SOURCE_OPERAND};
  operand_t immediate_desc_operand = {"#-3", IMMEDIATE, DESTINATION_OPERAND};
  operand_t direct_source_operand = {"SYMBOL", DIRECT, SOURCE_OPERAND};
  operand_t direct_desc_operand = {"ANOTHERSYMBOL", DIRECT, DESTINATION_OPERAND};
  operand_t indirect_register_source_operand = {"*r3", INDIRECT_REGISTER, SOURCE_OPERAND};
  operand_t indirect_register_desc_operand = {"*r7", INDIRECT_REGISTER, DESTINATION_OPERAND};
  operand_t direct_register_source_operand = {"r0", DIRECT_REGISTER, SOURCE_OPERAND};
  operand_t direct_register_desc_operand = {"r1", DIRECT_REGISTER, DESTINATION_OPERAND};

  /* add support immediate as source, but not as destination */
  if (FALSE != IncorrectAddressingMethod("add",
                                         &immediate_source_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != IncorrectAddressingMethod("add",
                                        &immediate_desc_operand,
                                        cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* cmp supports anything */
  if (FALSE != IncorrectAddressingMethod("cmp",
                                         &immediate_source_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IncorrectAddressingMethod("cmp",
                                         &immediate_desc_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IncorrectAddressingMethod("cmp",
                                         &direct_source_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IncorrectAddressingMethod("cmp",
                                         &direct_desc_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IncorrectAddressingMethod("cmp",
                                         &indirect_register_source_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IncorrectAddressingMethod("cmp",
                                         &indirect_register_desc_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IncorrectAddressingMethod("cmp",
                                         &direct_register_source_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IncorrectAddressingMethod("cmp",
                                         &direct_register_desc_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* lea doesn't support immediate addressing */
  if (TRUE != IncorrectAddressingMethod("lea",
                                        &immediate_source_operand,
                                        cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != IncorrectAddressingMethod("lea",
                                        &immediate_desc_operand,
                                        cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* ...it does support direct addressing... */
  if (FALSE != IncorrectAddressingMethod("lea",
                                         &direct_source_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IncorrectAddressingMethod("lea",
                                         &direct_desc_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* ...but supports indirect addressing for destination operand only */
  if (TRUE != IncorrectAddressingMethod("lea",
                                        &indirect_register_source_operand,
                                        cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != IncorrectAddressingMethod("lea",
                                         &indirect_register_desc_operand,
                                         cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }


  return test_info;
}

test_info_t SymbolDefinedMoreThanOnceTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolDefinedMoreThanOnce");
  symbol_table_t *table = CreateSymbolTable();

  if (NULL == table) {
    perror("Couldn't init symbol table in SymbolDefinedMoreThanOnceTest");
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (SUCCESS != AddSymbol(table,"aaaa",100, CODE)) {
    perror("Couldn't add symbol in SymbolDefinedMoreThanOnceTest");
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (TRUE != SymbolDefinedMoreThanOnce("aaaa", table, cfg)) {
     RETURN_ERROR(TEST_FAILED);
  }

  if (FALSE != SymbolDefinedMoreThanOnce("bbbb", table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }

  DestroySymbolTable(table);
  return test_info;
}

test_info_t SymbolWasntDefinedTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolDefinedMoreThanOnce");
  symbol_table_t *table = CreateSymbolTable();

  if (NULL == table) {
    perror("Couldn't init symbol table");
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (SUCCESS != AddSymbol(table,"aaaa",100, DATA)) {
    perror("Couldn't add symbol");
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (FALSE != SymbolWasntDefined("aaaa",table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != SymbolWasntDefined("bbbb",table, cfg)){
     RETURN_ERROR(TEST_FAILED);
  }

  DestroySymbolTable(table);
  return test_info;
}

test_info_t SymbolAlreadyDefinedAsExternTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolAlreadyDefinedAsExtern");
  symbol_table_t *table = CreateSymbolTable();

  if (SUCCESS != AddExternalSymbol(table,"aaaa")) {
     RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (SUCCESS != AddSymbol(table,"bbbb", 100, CODE)) {
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (SUCCESS != ChangeSymbolToEntry(table,"bbbb")) {
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (SUCCESS != AddSymbol(table,"cccc",100, CODE)) {
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (FALSE == SymbolAlreadyDefinedAsExtern("aaaa", table, cfg)) {
     RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE == SymbolAlreadyDefinedAsExtern("cccc", table, cfg)) {
     RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE == SymbolAlreadyDefinedAsExtern("bbbb", table, cfg)) {
     RETURN_ERROR(TEST_FAILED);
  }

  DestroySymbolTable(table);
  return test_info;
}

test_info_t SymbolNameIsIllegalTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("SymbolNameIsIllegal");
  const char *space_in_between = "asc da";
  const char *invalid_symbol = "?-c?";
  const char *valid_symbol = "ascasv";
  const char *invalid_sprefix1 = "1da";
  const char *invalid_sprefix2 = " ?-c?";
  const char *valid_prefix = "aaaa";
  const char *exceed = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

  /* Illegal characters */
  if (TRUE != SymbolNameIsIllegal(space_in_between, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != SymbolNameIsIllegal(invalid_symbol, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != SymbolNameIsIllegal(valid_symbol, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* Prefix */
  if (TRUE != SymbolNameIsIllegal(invalid_sprefix1, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != SymbolNameIsIllegal(invalid_sprefix2, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != SymbolNameIsIllegal(valid_prefix, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* Exceeded character limit */
  if (TRUE != SymbolNameIsIllegal(exceed, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  return test_info;
}

test_info_t SymbolUsedAsAMacroTest(syntax_check_config_t *cfg){
  test_info_t test_info = InitTestInfo("SymbolUsedAsAMacroTest");
  macro_table_t *table = CreateMacroTable();

  if (NULL == table) {
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (SUCCESS != AddMacroIfUnique(table,"aaaa","definition")) {
    DestroyMacroTable(table);
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (TRUE != SymbolUsedAsAMacro("aaaa",table, cfg)) {
     RETURN_ERROR(TEST_FAILED);
  }

  if (FALSE != SymbolUsedAsAMacro("bbbb",table, cfg)) {
     RETURN_ERROR(TEST_FAILED);
  }

  DestroyMacroTable(table);
  return test_info;
}

test_info_t NoDefinitionForSymbolTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("NoDefinitionForSymbol");
  const char *definition = ".data 53, 31";
  const char *no_definition1 = "  ";
  const char *no_definition2 = "";

  if (FALSE != NoDefinitionForSymbol(definition, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != NoDefinitionForSymbol(no_definition1, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != NoDefinitionForSymbol(no_definition2, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}
test_info_t IdentifyDirectiveTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("IdentifyDirective");
  const char *exist = ".entry";
  const char *without_dot = "entry";
  const char *not_exist = "aaaa";
  const char *uppercase_directive = ".dAta";

  if (ENTRY_DIRECTIVE != IdentifyDirective(exist, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (INVALID_DIRECTIVE != IdentifyDirective(without_dot, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (INVALID_DIRECTIVE != IdentifyDirective(not_exist, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (INVALID_DIRECTIVE != IdentifyDirective(uppercase_directive, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

test_info_t IsIllegalDataParameterTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("IsIllegalDataParameterTest");
  const char *not_enough_commas = "13, 145 7";
  const char *too_many_commas = "13, , 15";
  const char *illegal_characters = "13, 32x4";
  const char *empty_parameters = "";
  const char *ok1 = "+32";
  const char *ok2 = "1, -2";
  const char *ok3 = "11, -222, 3333";

  if (TRUE != IsIllegalDataParameter(not_enough_commas, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != IsIllegalDataParameter(too_many_commas, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != IsIllegalDataParameter(illegal_characters, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != IsIllegalDataParameter(empty_parameters, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (FALSE != IsIllegalDataParameter(ok1, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (FALSE != IsIllegalDataParameter(ok2, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (FALSE != IsIllegalDataParameter(ok3, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

test_info_t IsIllegalStringTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("IsIllegalStringTest");
  const char *legal = "\"abc\"";
  const char *illegal1 = "abc\"";
  const char *illegal2 = "\"abc";

  if (FALSE != IsIllegalString(legal, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != IsIllegalString(illegal1, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != IsIllegalString(illegal2, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

test_info_t RegisterNameDoesntExistTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("RegisterNameDoesntExist");
  const char *valid = "r0";
  const char *not_exist = "r9";
  const char *not_a_register = "aaaabbbb";

  if (TRUE != RegisterNameDoesntExist(not_a_register, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != RegisterNameDoesntExist(not_exist, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (FALSE != RegisterNameDoesntExist(valid, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

test_info_t DetectAddressingMethodTest(syntax_check_config_t *cfg) {
  test_info_t test_info = InitTestInfo("DetectExtraCharacters");
  const char *valid_register = "r7";
  const char *valid_indirect_register = "*r4";
  const char *invalid_indirect_register = "*r";
  const char *valid_immediate = "#+3";
  const char *invalid_immediate1 = "#+";
  const char *invalid_immediate2 = "#x";
  const char *invalid_immediate3 = "#";
  const char *valid_symbol = "SYMBOL";
  const char *symbol_looks_like_register = "r12";
  const char *invalid_symbol1 = "1SYMBOL";
  const char *invalid_symbol2 = "X012345678901234567890123456789XY";
  const char *invalid_symbol3 = " SYMBOL   ";

  if (DIRECT_REGISTER != DetectAddressingMethod(valid_register)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (INDIRECT_REGISTER != DetectAddressingMethod(valid_indirect_register)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (INVALID != DetectAddressingMethod(invalid_indirect_register)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (IMMEDIATE != DetectAddressingMethod(valid_immediate)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (INVALID != DetectAddressingMethod(invalid_immediate1)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (INVALID != DetectAddressingMethod(invalid_immediate2)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (INVALID != DetectAddressingMethod(invalid_immediate3)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (DIRECT != DetectAddressingMethod(valid_symbol)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (DIRECT != DetectAddressingMethod(symbol_looks_like_register)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (INVALID != DetectAddressingMethod(invalid_symbol1)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (INVALID != DetectAddressingMethod(invalid_symbol2)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (INVALID != DetectAddressingMethod(invalid_symbol3)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

test_info_t AreCommasMisplacedTest(syntax_check_config_t *cfg) { 
  test_info_t test_info = InitTestInfo("AreCommasMisplaced");
  char *valid1 = "S1, S2";
  char *valid2 = " S1,    S2   ";
  char *valid3 = "S1";
  char *valid4 = "";
  char *invalid1 = ",S1";
  char *invalid2 = "S1,";
  char *invalid3 = "S1, S1 a  , X";
  char *invalid4 = "S1,  , X";
  char *invalid5 = ",";

  if (FALSE != AreCommasMisplaced(valid1, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != AreCommasMisplaced(valid2, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != AreCommasMisplaced(valid3, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (FALSE != AreCommasMisplaced(valid4, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  if (TRUE != AreCommasMisplaced(invalid1, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != AreCommasMisplaced(invalid2, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != AreCommasMisplaced(invalid3, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != AreCommasMisplaced(invalid4, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }
  if (TRUE != AreCommasMisplaced(invalid5, cfg)) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

int main(int argc, char *argv[]) {
  int total_failures = 0;
  syntax_check_config_t cfg;
  test_info_t test_info;
  bool_t verbose = FALSE;
  int i = 0;

  test_func_t tests[] = {
    DetectExtraCharactersTest,
    IsReservedNameTest,
    MacroDefinedMoreThanOnceTest,
    InstructionDoesntExistTest,
    WrongNumberOfOperandsTest,
    DetectAddressingMethodTest,
    IncorrectAddressingMethodTest,
    SymbolDefinedMoreThanOnceTest,
    SymbolWasntDefinedTest,
    SymbolAlreadyDefinedAsExternTest,
    SymbolNameIsIllegalTest,
    SymbolUsedAsAMacroTest,
    NoDefinitionForSymbolTest,
    IsIllegalDataParameterTest,
    IsIllegalStringTest,
    RegisterNameDoesntExistTest,
    AreCommasMisplacedTest
  };

  /* Check if -v has been passed to enable verbose mode */
  for (i = 0; i < argc; ++i) {
    if (0 == strcmp(argv[i], "-v")) {
      verbose = TRUE;
    }
  }
  cfg = CreateSyntaxCheckConfig(argv[0], 0, verbose);

  /* Run tests */
  for (i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
    test_info = tests[i](&cfg);
    if (TEST_SUCCESSFUL != test_info.result) {
      PrintTestInfo(test_info);
      ++total_failures;
    }
  }

  if (0 == total_failures) {
    printf(BOLD_GREEN "Test successful: " COLOR_RESET "Syntax errors\n");
  }
  return total_failures;
}
