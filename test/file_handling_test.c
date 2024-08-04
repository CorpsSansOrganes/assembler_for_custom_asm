#include <string.h> /* strlen */
#include <stdio.h> /* printf */
#include "file_handling.h"

typedef enum {
  TEST_SUCCESSFUL,
  TEST_FAILED,
  TECHNICAL_ERROR
} test_result_t;

test_result_t TestHandleFiles(char *file_path, char *expected_content) {
  result_t result = StringToFile(file_path,
                                 expected_content,
                                 strlen(expected_content));
  char *file_content = NULL;

  if (SUCCESS != result) {
    printf("Couldn't perform test 'TestHandleFiles' - couldn't write string to file\n");
    return TECHNICAL_ERROR;
  }

  file_content = FileToString(file_path);
  if (NULL == file_content) {
    printf("Couldn't perform test 'TestHandleFiles' - couldn't read file to string\n");
    return TECHNICAL_ERROR;
  }

  if (strcmp(expected_content, file_content)) {
    printf("The content written to the file and the content read from the file aren't the same!\n");
    return TEST_FAILED;
  }

  return TEST_SUCCESSFUL;
}

int main(void) {
  char *asm_program = 
    "section .data\n"
    "    hello db 'Hello, World!', 0\n"
    "section .text\n"
    "    global _start\n"
    "_start:\n"
    "    ; write(1, hello, 13)\n"
    "    mov eax, 4         ; sys_write system call number\n"
    "    mov ebx, 1         ; file descriptor 1 (stdout)\n"
    "    mov ecx, hello     ; pointer to the string\n"
    "    mov edx, 13        ; length of the string\n"
    "    int 0x80           ; call kernel\n"
    "\n"
    "    ; exit(0)\n"
    "    mov eax, 1         ; sys_exit system call number\n"
    "    xor ebx, ebx       ; exit code 0\n"
    "    int 0x80           ; call kernel\n";

  test_result_t result = TestHandleFiles("./test/test.txt", asm_program);
  if (result) {
    printf("TestHandleFiles: Test failed!\n");
  } else {
    printf("TestHandleFiles: Test succesful.\n");
  }

  return result;
}
