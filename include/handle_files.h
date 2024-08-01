#ifndef __HANDLE_FILES__
#define __HANDLE_FILES__

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
char *FileToString(char *file_path);

/*
 * @brief Writes the content of a string into a file.
 *
 * @param file_path The path of the file we wish to write to.
 * @param string The string we wish to write into the file.
 *
 * @return An appropriate error code upon error, or SUCCESS.
 */
result_t StringToFile(char *file_path, char *string);

#endif /* __HANDLE_FILES__ */
