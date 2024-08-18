#include <stdio.h> /* fopen, close */
#include <string.h> /* strcmp */
#include <unistd.h> /* access */
#include "assembler.h"
#include "preprocessing.h"
#include "language_definitions.h"
#include "test_utils.h"
#include "macro_table.h"
#include "generate_output_files.h"

static result_t CompareFiles(const char *file1_path, const char *file2_path);

static const char *ProduceFilePath(const char *dir_path,
                                   const char *file_name,
                                   const char *extension,
                                   char *full_path);

static bool_t FileDoesntExist(const char *path);

static result_t RunComparisonOb(const char *file_name);
static result_t RunComparisonExt(const char *file_name);
static result_t RunComparisonEnt(const char *file_name);

const char *expected_dir = "./test/assembler_test_files/expected";
const char *input_dir = "./test/assembler_test_files/input";
const char *output_dir = "./test/assembler_test_files/output";


/*
 * TESTS
 */

test_info_t ValidAssemblingingTest(const char *file_name) {
  test_info_t test_info = InitTestInfo("ValidAssembling");
  char input_path[256];
  char ob_output_path[256];
  char ext_output_path[256];
  char ent_output_path[256];
  macro_table_t *default_macro_table = CreateMacroTable();

  ProduceFilePath(input_dir, file_name, ".am", input_path);
  ProduceFilePath(input_dir, file_name, ".ob", ob_output_path);
  ProduceFilePath(input_dir, file_name, ".ent", ent_output_path);
  ProduceFilePath(input_dir, file_name, ".ext", ext_output_path);

  if (SUCCESS != AssembleFile(input_path, default_macro_table)){
    printf("%s failed to assemble \n", file_name);
    RETURN_ERROR (TEST_FAILED);
  }

  if (SUCCESS != RunComparisonOb(file_name)) {
    printf("%s failed in ob\n", file_name);
    RETURN_ERROR(TEST_FAILED);
  }

  if (SUCCESS != RunComparisonExt(file_name)) {
    printf("%s failed in ext\n", file_name);
    RETURN_ERROR(TEST_FAILED);
  }  

  if (SUCCESS != RunComparisonEnt(file_name)) {
    printf("%s failed in ent\n", file_name);
    RETURN_ERROR(TEST_FAILED);
  }

  DestroyMacroTable(default_macro_table);
  return test_info;
}

test_info_t InvalidAssemblingTest(const char *file_name) {
  test_info_t test_info = InitTestInfo("InvalidAssembling");
  char preprocessing_path[256];
  char assembler_input_path[256];
  char output_path[256];
  macro_table_t *macro_table = NULL;

  ProduceFilePath(input_dir, file_name, ".as", preprocessing_path);
  ProduceFilePath(input_dir, file_name, ".am", assembler_input_path);/*TO check input dir*/
  ProduceFilePath(output_dir, file_name, ".ob", output_path);
  macro_table = PreprocessFile(preprocessing_path, assembler_input_path);

  if (NULL == macro_table) {
    printf("Preprocessing failed for '%s'\n", preprocessing_path);
    RETURN_ERROR(TECHNICAL_ERROR);
  }

  if (FALSE == FileDoesntExist(output_path)) {
    printf("%s failed - output file was created, although preprocessing failed.\n", file_name);
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

int main(void) {
  int total_failures = 0;
  size_t i = 0;

  char *valid_names[] = {
    "valid_1_only_data_definition",
    "valid_2_with_string_definition",
    "valid_3_with_instruction_0operand",
    "valid_4_with_instruction_1operand",
    "valid_5_with_instruction_2operands",
    "valid_6_with_symbol_data",
    "valid_7_with_symbol_instruction",
    "valid_8_with_external_symbol",
    "valid_9_with_entry",
    "valid_10_with_ignore_symbol_before_extern"
  };

    /*
  char *invalid_names[] = {
    "invalid_1_instruction_errors",
    "invalid_2_symbol_errors",
    "invalid_3_instruction_errors"
  };
    */

  for (i = 0 ; i < sizeof(valid_names) / sizeof(valid_names[0]); ++i) {
    test_info_t test_info = ValidAssemblingingTest(valid_names[i]);
    if (!WasTestSuccessful(test_info)) {
      PrintTestInfo(test_info);
      ++total_failures;
    }
  }

  /*
  for (i = 0; i < sizeof(invalid_names) / sizeof(invalid_names[0]); ++i) {
    test_info_t test_info = InvalidAssemblingTest(invalid_names[i]);
    if (!WasTestSuccessful(test_info)) {
      PrintTestInfo(test_info);
      ++total_failures;
    }
  }
  */

  if (0 == total_failures) {
    printf(BOLD_GREEN "Test successful: " COLOR_RESET "Assembler\n");
  }

  return total_failures;
}

/* 
 * STATIC FUNCTIONS
 */
static result_t CompareFiles(const char *output_file_path, const char *expected_file_path) {
  FILE *output_file = NULL;
  FILE *expected_file = NULL;
  char line1[MAX_LINE_LENGTH];
  char line2[MAX_LINE_LENGTH];
  int line_number = 1;
  result_t test_res = SUCCESS;

  if (FileDoesntExist(output_file_path) && FileDoesntExist (expected_file_path)){
    return SUCCESS;
  }

  output_file = fopen(output_file_path, "r");
  if (NULL == output_file) {
    perror("Error opening file");
    return FILE_HANDLING_ERROR;
  }


  expected_file = fopen(expected_file_path, "r");
  if (NULL == expected_file) {
    perror("Error opening file");
    fclose(output_file);  
    return FILE_HANDLING_ERROR;
  }


  while (fgets(line1, sizeof(line1), output_file) && fgets(line2, sizeof(line2), expected_file)) {
      if (0 != strcmp(line1, line2)) {
        fprintf(stderr, "Difference found on line %d:\n", line_number);
        fprintf(stderr, "\033[3;30;42m"); /* Background color to highlight differences. */
        fprintf(stderr, "Output:   \"%s\"\n", line1);
        fprintf(stderr, "Expected: \"%s\"\n", line2);
        printf(COLOR_RESET);       
        test_res = FAILURE;
      }
      ++line_number;
  }

  /* Check if one file has more lines than the other */
  if (fgets(line1, sizeof(line1), output_file) != NULL || fgets(line2, sizeof(line2), expected_file) != NULL) {
    printf("Files differ in length starting at line %d.\n", line_number + 1);
    test_res = FAILURE;
  }

  fclose(output_file);
  fclose(expected_file);
  return test_res;
}

static result_t RunComparisonOb(const char *file_name) {
  char output_file_path_ob[256];
  char expected_file_path_ob[256];

  ProduceFilePath(input_dir, file_name, ".ob", output_file_path_ob);
  ProduceFilePath(expected_dir, file_name, ".ob", expected_file_path_ob); 
  return CompareFiles(output_file_path_ob, output_file_path_ob);
}

static result_t RunComparisonExt (const char *file_name) {
  char output_file_path_ext[256];
  char expected_file_path[256];

  ProduceFilePath(input_dir, file_name, ".ext", output_file_path_ext);
  ProduceFilePath(expected_dir, file_name, ".ext", expected_file_path);
  return CompareFiles(output_file_path_ext, expected_file_path);
}

static result_t RunComparisonEnt(const char *file_name) {
  char output_file_path_ent[256];
  char expected_file_path[256];

  ProduceFilePath(input_dir, file_name, ".ent", output_file_path_ent);
  ProduceFilePath(expected_dir, file_name, ".ent", expected_file_path);
  return CompareFiles(output_file_path_ent, expected_file_path);
}

static const char *ProduceFilePath(const char *dir_path,
                                   const char *file_name,
                                   const char *extension,
                                   char *full_path) {
    sprintf(full_path, "%s/%s%s", dir_path, file_name, extension);
    return full_path;
}

static bool_t FileDoesntExist(const char *path) {
  return (0 != access(path, F_OK));
}
