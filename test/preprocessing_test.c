#include <stdio.h> /* fopen, close */
#include <string.h> /* strcmp */
#include "preprocessing.h"
#include "language_definitions.h"
#include "test_utils.h"

result_t static CompareFiles(const char *file1_path, const char *file2_path);

int main(void) {
  int total_failures = 0;
  const char *result_path;
  const char *expected_path;

  if (0 == total_failures) {
    printf(BOLD_GREEN "Test successful: " COLOR_RESET "Preprocessing\n");
  }

  return total_failures;
}

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

  // Check if one file has more lines than the other
  if (fgets(line1, sizeof(line1), file1) != NULL || fgets(line2, sizeof(line2), file2) != NULL) {
    printf("Files differ in length starting at line %d.\n", line_number + 1);
    test_res = FAILURE;
  }

  fclose(file1);
  fclose(file2);
  return test_res;
}
