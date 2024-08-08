#ifndef __STRING_UTILS__
#define __STRING_UTILS__

/*
 * Functions for manipulating & analysing strings, which aren't present in <string.h>
 */

#include "utils.h" /* bool_t */

/*
 * @brief Tells if a string starts with a certain prefix.
 *
 * @param str - Null-terminated string, to check for the prefix.
 *        prefix - The prefix to look for at the beginning of `str`. Must be
 *                 null-terminated.
 *
 * @return TRUE if 'str' starts with 'prefix'. FALSE otherwise.
 */

bool_t IsPrefix(const char *str, const char *prefix);

/*
 * @brief Removes leading & trailing whitespaces from a string.
 *
 * For example:
 * "   hello w   orld     " -> "hello w   orld"
 *
 * Whitespaces are considered spaces and tabs.
 *
 * @param str - Null-terminated string to strip. 
 *              NOTE: this string will be modified by the function in-place.
 *
 * @return Pointer to the start of the modified string.
 */

char *StripWhitespaces(char *str);

/*
 * @brief Same as StripWhitespaces, but only removes leading whitespaces.
 *
 * @param - str - Null-terminated string to strip.
 *
 * @return Pointer to the start of the stripped string.
 */
const char *StripLeadingWhitespaces(const char *str);

/*
 * @brief Same as StripWhitespaces, but only removes trailing whitespaces.
 *
 * @param str - Null-terminated string to strip.
 *              NOTE: this string will be modified by the function in-place.
 *
 * @return Pointer to the start of the stripped string.
 */
char *StripTrailingWhitespaces(char *str);

/*
 * @brief Returns a pointer to the end of the string.
 * 
 * This function returns a pointer to the null terminator of the given string.
 * 
 * @param str - Pointer to the null-terminated string.
 * 
 * @return Pointer to the null terminator of the string.
 */

const char *EndOfString(const char *str);

#endif /* __STRING_UTILS__ */
