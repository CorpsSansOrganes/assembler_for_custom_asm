#include <stdio.h> /* fopen, close */
#include <string.h> /* strcmp */
#include "preprocessing.h"
#include "language_definitions.h"
#include "test_utils.h"

result_t static CompareFiles(const char *file1_path, const char *file2_path);

static result_t RunComparison(const char *file_name);

static const char *ProduceFilePath(const char *dir_path,
                                   const char *file_name,
                                   const char *extension,
                                   char *full_path);

const char *expected_dir = "./test/preprocessing_test_files/expected";
const char *input_dir = "./test/preprocessing_test_files/input";
const char *output_dir = "./test/preprocessing_test_files/output";

/*
 * TESTS
 */
test_info_t ValidPreprocessingTest(const char *file_name) {
  test_info_t test_info = InitTestInfo("ValidPreprocessing");
  char input_path[256];
  char output_path[256];
  macro_table_t *table = NULL;

  ProduceFilePath(input_dir, file_name, ".as", input_path);
  ProduceFilePath(output_dir, file_name, ".am", output_path);

  table = PreprocessFile(input_path, output_path);

  if (SUCCESS != RunComparison(file_name)) {
    printf("%s failed\n", file_name);
    RETURN_ERROR(TEST_FAILED);
  }

  DestroyMacroTable(table);
  return test_info;
}

int main(void) {
  int total_failures = 0;
  int i = 0;

  char *valid_names[] = {
    "valid_1_wo_macro"
  };

  for (i = 0 ; i < sizeof(valid_names) / sizeof(valid_names[0]); ++i) {
    test_info_t test_info = ValidPreprocessingTest(valid_names[i]);
    if (!WasTestSuccessful(test_info)) {
      PrintTestInfo(test_info);
      ++total_failures;
    }
  }

  if (0 == total_failures) {
    printf(BOLD_GREEN "Test successful: " COLOR_RESET "Preprocessing\n");
  }

  return total_failures;
}

/* 
 * STATIC FUNCTIONS
 */
result_t static CompareFiles(const char *file1_path, const char *file2_path) {
  FILE *file1 = fopen(file1_path, "r");
  FILE *file2 = NULL;
  char line1[MAX_LINE_LENGTH];
  char line2[MAX_LINE_LENGTH];
  int line_number = 1;
  result_t test_res = SUCCESS;

  if (NULL == file1) {
    perror("Error opening file");
    return FILE_HANDLING_ERROR;
  }


  file2 = fopen(file2_path, "r");
  if (NULL == file2) {
    perror("Error opening file");
    fclose(file1);  
    return FILE_HANDLING_ERROR;
  }


  while (fgets(line1, sizeof(line1), file1) && fgets(line2, sizeof(line2), file2)) {
      if (0 != strcmp(line1, line2)) {
        fprintf(stderr, "Difference found on line %d:\n", line_number);
        fprintf(stderr, "File 1: %s\n", line1);
        fprintf(stderr, "File 2: %s\n", line2);
        test_res = FAILURE;
      }
      ++line_number;
  }

  /* Check if one file has more lines than the other */
  if (fgets(line1, sizeof(line1), file1) != NULL || fgets(line2, sizeof(line2), file2) != NULL) {
    printf("Files differ in length starting at line %d.\n", line_number + 1);
    test_res = FAILURE;
  }

  fclose(file1);
  fclose(file2);
  return test_res;
}

static result_t RunComparison(const char *file_name) {
  char input_file_path[256];
  char expected_file_path[256];
  ProduceFilePath(input_dir, file_name, ".as", input_file_path);
  ProduceFilePath(expected_dir, file_name, ".am", expected_file_path);
  
  return CompareFiles(input_file_path, expected_file_path);
}

static const char *ProduceFilePath(const char *dir_path,
                                   const char *file_name,
                                   const char *extension,
                                   char *full_path) {
    sprintf(full_path, "%s/%s%s", dir_path, file_name, extension);
    return full_path;
}
