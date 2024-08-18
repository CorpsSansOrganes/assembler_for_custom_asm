/* assembler.c
 *
 * This module handles the main process of the assembler,
 * including the first and second passes.
 * It utilizes the 'generate_opcode' module to compute opcodes,
 * and finally calls 'generate_output_files' to produce the output.
 */

#include <stdio.h> /* fopen, fclose */
#include <string.h> /* strlen */
#include <stdlib.h> /* malloc, free */
#include "assembler.h"
#include "syntax_errors.h"
#include "symbol_table.h"
#include "language_definitions.h"
#include "generate_opcode.h"
#include "list.h"
#include "string_utils.h"
#include "generate_output_files.h"

const char *DELIMITERS = ", \t\n\r";

/* @brief Checks if the first word in a string ends with ':'.
 * @param Line - Line of text to check.
 * @return TRUE if ends with colon, otherwise FALSE.
 */

static bool_t IsSymbolDefinition(const char *line) {
  while ('\n' != *(line + 1) && !IsBlank(*(line + 1))) {
    line++;
  }
  if (*line == ':') {
    return TRUE;
  }
  return FALSE;
}

/*
 * @brief Checks all possible syntax errors of symbols.
 *
 * @param symbol_name - The name of the symbol, e.g. "SYMBOL".
 *        macro_table - Macro table of the file.
 *        symbol_table - Symbol table of the file.
 *        cfg - Syntax error configurations, see in syntax_errors.h.
*/

static bool_t SymbolNameErrorOccurred(const char *symbol_name,
                                  macro_table_t *macro_table,
                                  symbol_table_t *symbol_table,
                                  syntax_check_config_t *cfg) {
  int internal_counter = 0;
  if (SymbolNameIsIllegal(symbol_name,cfg)) {
    internal_counter++;
  }
  if (SymbolUsedAsAMacro(symbol_name, macro_table, cfg)) {
    internal_counter++;
  }
  if (SymbolDefinedMoreThanOnce(symbol_name,symbol_table, cfg)) {
    internal_counter++;
  }

  return internal_counter ? TRUE : FALSE;
}


/*
 * @brief Increments the addresses associated with all data symbols with 100 + IC
 */

static void UpdateDataSymbolsAddresses(symbol_table_t *symbol_table, unsigned int IC) {
  list_t *symbol_list = AsList(symbol_table);
  node_t *iter = GetHead(symbol_list);

  while (NULL != iter) {
    symbol_t *symbol = (symbol_t *)GetValue(iter);
    if (DATA == GetSymbolMemoryArea(symbol)) {
      UpdateSymbolAddress(symbol,
                          GetSymbolAddress(symbol) + IC + INITIAL_IC_VALUE);
    }

    iter = GetNext(iter);
  }
}

/*
 * @brief Read operands passed to an instruction, count them & return the first two.
 */
static int SplitOperands(char *line, operand_t *operand1, operand_t *operand2) {
  int counter = 0;
  char *current_word = strtok(line, DELIMITERS);

  if (NULL != current_word) {
    operand1->name = StrDup(current_word);
    if (NULL == operand1->name) {
      return -1;

    }
    operand1->addressing_method = DetectAddressingMethod(operand1->name);
    operand1->type = DESTINATION_OPERAND;
    counter++;
  }

  current_word = strtok(NULL, DELIMITERS);
  if (NULL != current_word) {
    operand2->name = StrDup (current_word);
    if (NULL == operand2->name) {
      free((void *)operand1->name);
      operand1->name = NULL;
      return -1;
    }
    operand2->addressing_method = DetectAddressingMethod(operand2->name);
    operand2->type = DESTINATION_OPERAND;
    operand1->type = SOURCE_OPERAND;
    counter++;

    /* Count extra parameters */
    current_word = strtok(NULL, DELIMITERS);
    while (NULL != current_word) {
      counter++;
      current_word = strtok(NULL, DELIMITERS);
    }
  }
  return counter;
}  

/*
 * @param instruction - Instruction name (e.g. "mov")
 *        params - String containing the operands passed to the instruction.
 *                 If NULL, there are 0 parameters (e.g. "#+3, SYM1").
 */
static result_t HandleInstructionStatement(char *instruction,
                                           char *params,
                                           symbol_table_t *symbol_table,
                                           char *symbol_name,
                                           vector_t *code_table,
                                           syntax_check_config_t *cfg) {

  int operand_num = 0;
  operand_t operands[2] = {
    {NULL, INVALID, SOURCE_OPERAND},
    {NULL, INVALID, SOURCE_OPERAND}
  };

  bool_t invalid_operands = FALSE;
  operand_t *src_operand = NULL;
  operand_t *dest_operand = NULL;
  int i = 0;

  if (InstructionDoesntExist(instruction, cfg)) {
    return FAILURE;
  }

  if (NULL != params) {
    operand_num = SplitOperands(params, &operands[0], &operands[1]);
  }

  if (-1 == operand_num) {
    return MEM_ALLOCATION_ERROR;
  }

  /* Syntax errors for operands */
  if (WrongNumberOfOperands(instruction, operand_num, cfg)) {
    if (NULL != operands[0].name) {
      free((void *)operands[0].name);
    }
    if (NULL != operands[1].name) {
      free((void *)operands[1].name);
    }
    return FAILURE;
  }

  for (i = 0; i < operand_num; ++i) {
    if (IsOperandInvalid(&operands[i], cfg)) {
      invalid_operands = TRUE;
    }

    else if (IncorrectAddressingMethod(instruction, &operands[0], cfg)) {
      invalid_operands = TRUE;
    }

    else if (IMMEDIATE == operands[i].addressing_method &&
        ImmediateOperandTooBig(&operands[i], cfg)) {
      invalid_operands = TRUE;
    }
  }

  if (invalid_operands) {
    if (NULL != operands[0].name) {
      free((void *)operands[0].name);
    }
    if (NULL != operands[1].name) {
      free((void *)operands[1].name);
    }
    return FAILURE;
  }

  /* Create symbol, if one was defined */
  if (NULL != symbol_name) {
    if (SUCCESS != AddSymbol(symbol_table,
                             symbol_name,
                             GetSizeVector(code_table) + INITIAL_IC_VALUE,
                             CODE)) {

    if (NULL != operands[0].name) {
      free((void *)operands[0].name);
    }
    if (NULL != operands[1].name) {
      free((void *)operands[1].name);
    }
      return MEM_ALLOCATION_ERROR;
    }
  }

  /* Generate machine code in the code segment */
  if (1 == operand_num) {
    dest_operand = &operands[0];
  }
  if (2 == operand_num) {
    src_operand = &operands[0];
    dest_operand = &operands[1];
  }

  if (SUCCESS != InstructionStatementToMachinecode(code_table,
                                                   instruction,
                                                   src_operand,
                                                   dest_operand)) {

    if (NULL != operands[0].name) {
      free((void *)operands[0].name);
    }
    if (NULL != operands[1].name) {
      free((void *)operands[1].name);
    }
    return MEM_ALLOCATION_ERROR;
  }

  if (NULL != operands[0].name) {
    free((void *)operands[0].name);
  }
  if (NULL != operands[1].name) {
    free((void *)operands[1].name);
  }

  return SUCCESS;
}

static result_t HandleStringOrData(directive_t directive,
                                   char *param,
                                   symbol_table_t *symbol_table,
                                   char *symbol_name,
                                   vector_t *data_table,
                                   syntax_check_config_t *cfg) {

  typedef bool_t (*syntax_check_func_t)(const char *, syntax_check_config_t *);
  typedef result_t (*machine_code_generation_func_t)(vector_t *, char *);

  size_t to_skip = 0;
  syntax_check_func_t syntax_check_func = NULL;
  machine_code_generation_func_t generate_machine_code = NULL;

  if (STRING_DIRECTIVE == directive) {
    to_skip = strlen(".string") + 1;
    syntax_check_func = IsIllegalString;
    generate_machine_code = StringDirectiveToMachinecode;
  }
  else if (DATA_DIRECTIVE == directive) { 
    to_skip = strlen(".data") + 1;
    syntax_check_func = IsIllegalDataParameter;
    generate_machine_code = DataDirectiveToMachinecode;
  }

  /* Skip to directive's parameter */
  param += to_skip;
  
  /* Check syntax errors in parameters */
  param = strtok(param, "\n"); 
  if (syntax_check_func(param, cfg)) {
    return FAILURE; 
  }

  /* Create symbol, if one was defined */
  if (NULL != symbol_name) {
    if (SUCCESS != AddSymbol(symbol_table,
                             symbol_name,
                             GetSizeVector(data_table),
                             DATA)) {

      return MEM_ALLOCATION_ERROR;
    }
  }

  /* Generate machine code in the data segment */
  if (SUCCESS != generate_machine_code(data_table, param)) {
    return MEM_ALLOCATION_ERROR;
  }

  return SUCCESS;
}

static result_t HandleDirectiveStatement(char *directive_name,
                                         char *params,
                                         macro_table_t *macro_table,
                                         symbol_table_t *symbol_table,
                                         vector_t *data_table,
                                         char *symbol_name,
                                         syntax_check_config_t *cfg) {

  /*
   * First, check if that directive even exists
   */
  directive_t directive = IdentifyDirective(directive_name, cfg);
  if (INVALID_DIRECTIVE == directive) {
    return FAILURE;
  }
  
  /*
   * Is it a .string or .data directive?
   * e.g. "SYMBOL: .string ...", ".data ..."
   */
  else if (STRING_DIRECTIVE == directive || DATA_DIRECTIVE == directive) {
      return HandleStringOrData(
        directive,
        directive_name,
        symbol_table,
        symbol_name,
        data_table,
        cfg
    );
  }  

  /*
   * Is it a .extern or .entry directive?
   * e.g. "SYMBOL: .extern...", ".entry..."
   */
  else {
    /* If symbol was defined, it warrants a warning. */
    if (NULL != symbol_name) {
      printf(BOLD_YELLOW "WARNING: " COLOR_RESET "(file %s, line %u):\n label before .extern or .entry is invalid\n\n",
             cfg->file_name,
             cfg->line_number);
    }

    if (EXTERN_DIRECTIVE == directive) {
      /* Check if commas are misplaced in the parameters passed to .extern */
      if (AreCommasMisplaced(params, cfg)) {
        return FAILURE;
      }

      /* Adding each symbol passed as a parameter to .extern to the symbol
       * table as an external table.
       */
      params = strtok(params, DELIMITERS);
      while (NULL != params) {
        /* Check syntax errors for each symbol name */
        if (TRUE == SymbolNameErrorOccurred(params,
                                            macro_table,
                                            symbol_table,
                                            cfg)) {
          return FAILURE;
        }

        if (SUCCESS != AddExternalSymbol(symbol_table, params)) {
          return MEM_ALLOCATION_ERROR;
        }

        params = strtok(NULL,DELIMITERS);
      }
    }

    else if (ENTRY_DIRECTIVE == directive) {
      /* Do nothing. Entries will be handled in 2nd pass */
    }
  }
  
  return SUCCESS;
}

/*
 * @brief Performs a first pass on the code:
 *        1. Creating symbol table from symbol declarations.
 *        2. Creating initial memory mapping, which will be completed in the
 *           second pass.
 *
 * @param file_path - The file to read & perform first pass on.
 *
 *        macro_table - Table of all macros identified in the preprocessing.
 *
 *        symbol_table - A valid empty symbol table object, which will be populated
 *                       during first pass.
 *
 *        code_table - A valid empty vector which will contain the machine code
 *                     words corresponding to the assembly instruction statements.
 *        
 *        data_table - A valid empty vector which will contain the machine code
 *                     encoding of directive statements.
 *
 * @return SUCCESS if no syntax error or other fault occurred.
 *         FAILURE if one or more syntax errors occurred.
 *         MEM_ALLOCATION_ERROR if a memory allocataion error occurred.
 */

static result_t FirstPass(char *file_path,
                          macro_table_t *macro_table,
                          symbol_table_t *symbol_table,
                          vector_t *code_table,
                          vector_t *data_table) {
  int total_errors = 0;
  char *current_word = NULL; 
  char *current_line = NULL;
  char *symbol_name = NULL;
  FILE *input_file = NULL;
  syntax_check_config_t cfg = CreateSyntaxCheckConfig(file_path, 0, TRUE);

  /* Acquire resources */
  input_file = fopen(file_path, "r");
  if (NULL == input_file) {
    fprintf(stderr,"Couldn't open input file '%s'.\n", file_path);
    return ERROR_OPENING_FILE; 
  }

  current_line = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
  if (NULL == current_line) {
    fclose (input_file);
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;
  }

  /* ~ * ~ ------------------------------ ~ * ~
   * Performing syntax analysis for each line.
   * ~ * ~ ------------------------------ ~ * ~
   */
  while (NULL != fgets(current_line, MAX_LINE_LENGTH, input_file)) {
    ++cfg.line_number;
    symbol_name = NULL;

    /* 
     * Does the line begin with a symbol definition?
     * e.g. "SYMBOL: ..."
     */
    if (IsSymbolDefinition(current_line)) {
      /* Check if there's space after : */
      if (DefinitionStartsImmediatelyAfterColon(current_line, &cfg)) {
        total_errors++;
        continue;
      }

      /* Extract symbol name */
      current_word = strtok(current_line, ": \t\n\r");
      symbol_name = StrDup(current_word);

      if (NULL == symbol_name) {
        perror("Error: memory allocation error\n");
        free(current_line);
        fclose(input_file);
        return MEM_ALLOCATION_ERROR;
      }
      
      if (SymbolNameErrorOccurred(symbol_name,
                                  macro_table,
                                  symbol_table,
                                  &cfg)) {
        total_errors++;
        free(symbol_name); symbol_name = NULL;
        continue;
      }

      /* Skip after the label */
      current_word = strtok(NULL, ": \t\n");
      if (NoDefinitionForSymbol(current_word, &cfg)){
        total_errors++;
        free(symbol_name); symbol_name = NULL;
        continue;
      }
    }        

    else {
      /* First word (no symbol definition) */
      current_word = strtok(current_line, DELIMITERS);
    }

    /* 
     * Handle directives
     * e.g. "SYMBOL: .string ..."
     *      ".extern ..."
     */
    if ('.' == *current_word) {
      char *params = strtok(NULL, "\n\0");
      result_t res = HandleDirectiveStatement(current_word,
                                              params,
                                              macro_table,
                                              symbol_table,
                                              data_table,
                                              symbol_name,
                                              &cfg);

      /* Symbol was added to symbol table, no longer needed */
      if (NULL != symbol_name) {
        free(symbol_name);
      }

      if (MEM_ALLOCATION_ERROR == res) {
        perror("Error: memory allocation error\n");
        free(current_line);
        fclose(input_file);
        return MEM_ALLOCATION_ERROR;
      }
      else if (FAILURE == res) {
        ++total_errors;
      }
    }

    /*
     * Handle instruction statements.
     * e.g. "SYMBOL: mov ... "
     *      "mov ..." 
     */
    else {
      char *params = strtok(NULL, "\n\0");
      result_t res = HandleInstructionStatement(current_word,
                                                params,
                                                symbol_table,
                                                symbol_name,
                                                code_table,
                                                &cfg);
      /* Symbol was added to the symbol table */
      if (NULL != symbol_name) {
        free(symbol_name);
      }
      if (MEM_ALLOCATION_ERROR == res) {
        perror("Error: memory allocation error\n");
        free(current_line);
        fclose(input_file);
        return MEM_ALLOCATION_ERROR;
      }
      else if (FAILURE == res) {
        ++total_errors;
      }
    }
  }

  free(current_line);
  fclose(input_file);

  if (total_errors) {
    return FAILURE;
  }

  UpdateDataSymbolsAddresses(symbol_table, GetSizeVector(code_table));
  return SUCCESS;
}

static result_t SecondPass(char *file_path,
                           symbol_table_t *symbol_table,
                           vector_t *code_table,
                           ext_symbol_occurrences_t *ext_list) {

  syntax_check_config_t cfg = CreateSyntaxCheckConfig(file_path, 0, TRUE);
  unsigned int IC = 0;
  int total_errors = 0;
  char *current_word = NULL; 
  char *current_line = NULL;
  FILE *input_file = NULL;
  
  /* Acquire resources */
  input_file = fopen(file_path, "r");
  if (NULL == input_file) {
    fprintf(stderr,"Couldn't open input file '%s'.\n", file_path);
    return ERROR_OPENING_FILE; 
  }

  current_line = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
  if (NULL == current_line) {
    fclose(input_file);
    fprintf(stderr,
            "Memory allocation error: couldn't allocate a buffer\n");
    return MEM_ALLOCATION_ERROR;
  }

  /* ~ * ~ ------------------------------ ~ * ~
   * Performing syntax analysis for each line.
   * ~ * ~ ------------------------------ ~ * ~
   */
  while (NULL != fgets(current_line, MAX_LINE_LENGTH, input_file)) {
    ++cfg.line_number;

    if (IsSymbolDefinition(current_line)) {
      /* If we have a symbol definition, e.g. "SYMBOL: ...", skip after the label */
      current_word = strtok(current_line, ": \t\n\r");
      current_word = strtok(NULL, ": \t");
    }
    else {
      /* Get the first word */
      current_word = strtok(current_line, DELIMITERS);
    }

    /*
     * Handle directives. 
     * The only one left to handle is .entry.
     */
    if ('.' == *current_word) {
      syntax_check_config_t silent_cfg = CreateSyntaxCheckConfig(NULL, 0, FALSE);
      if (ENTRY_DIRECTIVE != IdentifyDirective(current_word, &silent_cfg)) {
        /* Other directive have been handled in FirstPass */
        continue;
      }

      /* Check if commas are misplaced in the parameters passed to .entry*/
      if (AreCommasMisplaced(strstr(current_line, ".entry") + strlen(".entry "),
                             &cfg)) {
        return FAILURE;
      }

      /* Update each symbol passed as a parameter to .entry as an .entry symbol.
       * Check syntax errors for each symbol.
       */

      current_word = strtok(NULL, DELIMITERS);
      while (NULL != current_word) {
        if (SymbolNameIsIllegal(current_word, &cfg)) {
          total_errors++;
        }
        else if (SymbolWasntDefined(current_word, symbol_table, &cfg)) {
          total_errors++;
        }

        else if (SymbolAlreadyDefinedAsExtern(current_word, symbol_table, &cfg)) {
          total_errors++;
        }
        else {
          ChangeSymbolToEntry(symbol_table, current_word);
        }
        current_word = strtok (NULL,DELIMITERS);
      }
    }

    /* 
     * Handle instructions.
     * Guaranteed to be an instruction statement.
     */
    else {
      unsigned int register_operands_num = 0;
      int i = 0;
      ++IC;

      /* Skip the instruction to its operands */
      current_word = strtok(NULL, DELIMITERS);

      while (NULL != current_word && i < 2) {
        /* Read the operand */
        addressing_method_t method = DetectAddressingMethod(current_word);
        symbol_t *symbol = NULL;

        if (DIRECT == method) {
          symbol = FindSymbol(symbol_table, current_word);
        }
        
        /* Symbol used but wasn't defined */
        if (DIRECT == method && SymbolWasntDefined(current_word, symbol_table, &cfg)) {
          total_errors++;
        }

        /* Check if first operand is a symbol, if so update accordingly */
        else if (DIRECT == method && NULL != symbol) {

          /* If its extern add the occurence to the list for the .ext file */
          if (EXTERN == GetSymbolType (symbol)) {
            AddExternalSymbolOccurence(ext_list, GetSymbolName(symbol), IC + INITIAL_IC_VALUE);
          }

          /* If its a non-extern symbol we update the missing addresses in the code segment */
          else {
            bitmap_t *opcode_block = (bitmap_t *)GetElementVector(code_table, IC);
            *opcode_block = GetSymbolAddress(symbol);
          }
        } 

        else if (DIRECT_REGISTER == method || INDIRECT_REGISTER == method) {
          ++register_operands_num;
        }

        /* Skip to next operand */
        current_word = strtok (NULL,DELIMITERS);
        ++i;
        ++IC;
      }
      if (2 == register_operands_num) {
        --IC;
      }
    }
  }

  fclose (input_file);
  free (current_line);
  if (0 == total_errors) {
    return SUCCESS;
  }
  return FAILURE;
}


result_t AssembleFile(char *file_path, macro_table_t *macro_table) {
  result_t res = SUCCESS;
  bool_t no_errors = TRUE;

  /* Symbol table which will be populated with symbols in first pass */
  symbol_table_t *symbol_table = NULL;

  /* Two vectors for holding machine code for code & data segments, respectively. */
  vector_t *code_table = NULL;
  vector_t *data_table = NULL;

  /* List which will holds all places where an external symbol is used */
  ext_symbol_occurrences_t *ext_list = NULL;

  /*
   * Acquiring resources 
   */

  ext_list = CreateExternalSymbolList();
  if (NULL == ext_list) {
    fprintf(stderr,"Memory allocation error: couldn't allocate ext. symbol usage list\n");
    return MEM_ALLOCATION_ERROR;
  }

  symbol_table = CreateSymbolTable();
  if (NULL == symbol_table) {
    fprintf(stderr,"Memory allocation error: couldn't allocate a symbol table\n");
    DestroyExternSymbolList(ext_list);
    return MEM_ALLOCATION_ERROR;
  }  

  code_table = CreateVector(10, sizeof(vector_t *));
  if (NULL == code_table) {
    fprintf(stderr,"Memory allocation error: couldn't allocate a code table\n");
    DestroyExternSymbolList(ext_list);
    DestroySymbolTable(symbol_table);
    return MEM_ALLOCATION_ERROR;
  }  

  data_table = CreateVector(10, sizeof(vector_t *));
  if (NULL == data_table) {
    fprintf(stderr,"Memory allocation error: couldn't allocate a data table\n");
    DestroyExternSymbolList(ext_list);
    DestroySymbolTable(symbol_table);
    DestroyVector(code_table);
    return MEM_ALLOCATION_ERROR;
  }  

  /*
   * Assembler performing first & second pass
   */
  res = FirstPass(file_path,
                  macro_table,
                  symbol_table,
                  code_table, 
                  data_table);
  if (ERROR_OPENING_FILE == res) {
    DestroyExternSymbolList(ext_list);
    DestroySymbolTable(symbol_table);
    DestroyVector(code_table);
    DestroyVector(data_table);
    return ERROR_OPENING_FILE;
  }

  if (SUCCESS != res)  {
    no_errors = FALSE;
  }

  if (SUCCESS != SecondPass(file_path,
                          symbol_table,
                          code_table,
                          ext_list)) {
    no_errors = FALSE;

  }

  /*
   * Generating output files 
   */
  if (no_errors &&
      SUCCESS != GenerateOutputFiles(code_table,
                                     data_table,
                                     symbol_table,
                                     file_path,
                                     ext_list)) {
    no_errors = FALSE;
  }

  DestroyExternSymbolList(ext_list);
  DestroySymbolTable(symbol_table);
  DestroyVector(code_table);
  DestroyVector(data_table);
  return no_errors ? SUCCESS : FAILURE;
}
