#include <stdio.h> /* fopen, close */
#include <string.h> /* strcmp */
#include <unistd.h> /* access */
#include "preprocessing.h"
#include "language_definitions.h"
#include "test_utils.h"

static result_t CompareFiles(const char *file1_path, const char *file2_path);

static result_t RunComparison(const char *file_name);

static const char *ProduceFilePath(const char *dir_path,
                                   const char *file_name,
                                   const char *extension,
                                   char *full_path);

static bool_t FileDoesntExist(const char *path);
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

test_info_t InvalidPreprocessingTest(const char *file_name) {
  test_info_t test_info = InitTestInfo("InvalidPreprocessing");
  char input_path[256];
  char output_path[256];
  macro_table_t *table = NULL;

  ProduceFilePath(input_dir, file_name, ".as", input_path);
  ProduceFilePath(output_dir, file_name, ".am", output_path);

  table = PreprocessFile(input_path, output_path);

  if (NULL != table) {
    printf("%s failed - table isn't null although preprocessing failed.\n", file_name);
    DestroyMacroTable(table);
    RETURN_ERROR(TEST_FAILED);
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
    /*
    "valid_1_wo_macro",
    "valid_2_w_macro",
    */
    "valid_3_w_comment",
    /*
    "valid_4_w_blank_line",
    "valid_5_junky"
    */
  };

  
  /*
  char *invalid_names[] = {
    "invalid_1_extra_characters",
    "invalid_2_macro_is_reserved_name" 
  };
  */
  

  for (i = 0 ; i < sizeof(valid_names) / sizeof(valid_names[0]); ++i) {
    test_info_t test_info = ValidPreprocessingTest(valid_names[i]);
    if (!WasTestSuccessful(test_info)) {
      PrintTestInfo(test_info);
      ++total_failures;
    }
  }

  
  /*
  for (i = 0; i < sizeof(invalid_names) / sizeof(invalid_names[0]); ++i) {
    test_info_t test_info = InvalidPreprocessingTest(invalid_names[i]);
    if (!WasTestSuccessful(test_info)) {
      PrintTestInfo(test_info);
      ++total_failures;
    }
  }
  */
  

  if (0 == total_failures) {
    printf(BOLD_GREEN "Test successful: " COLOR_RESET "Preprocessing\n");
  }

  return total_failures;
}

/* 
 * STATIC FUNCTIONS
 */
static result_t CompareFiles(const char *output_file_path,
                             const char *expected_file_path) {
  FILE *output_file = fopen(output_file_path, "r");
  FILE *expected_file = NULL;
  char line1[MAX_LINE_LENGTH];
  char line2[MAX_LINE_LENGTH];
  int line_number = 1;
  result_t test_res = SUCCESS;

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

static result_t RunComparison(const char *file_name) {
  char output_file_path[256];
  char expected_file_path[256];
  ProduceFilePath(output_dir, file_name, ".am", output_file_path);
  ProduceFilePath(expected_dir, file_name, ".am", expected_file_path);
  
  return CompareFiles(output_file_path, expected_file_path);
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
