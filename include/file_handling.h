#ifndef __FILE_HANDLING__
#define __FILE_HANDLING__

#include <stdlib.h> /* size_t */
#include "utils.h"

/*
 * @brief Reads the entire content of a file into a string.
 *
 * @param file_path The path of the file we wish to read.
 *
 * @return Upon success, a pointer to a newly allocated string 
 *  into which the file's content have been written.
 *  Upon Failure, NULL is returned.
 */
char *FileToString(const char *file_path);

/*
 * @brief Writes the content of a string into a file.
 *
 * @param file_path - The path of the file we wish to write to.
 * @param string - The string we wish to write into the file.
 * @param length - The length of the string (excluding '\0').
 *
 * @return An appropriate error code upon error, or SUCCESS.
 */
result_t StringToFile(char *file_path, char *string, size_t length);

#endif /* __FILE_HANDLING__ */
