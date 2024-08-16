#ifndef __STRING_UTILS__
#define __STRING_UTILS__

#define  delimiters ", \t\n\r"


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
char *StripLeadingWhitespaces(char *str);

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

/*
 * @brief Copies a substring from the range [from, to) in a source string to dest.
 *
 * @param from, to - Two pointers in a source string, s.t. from < to.
 *        dest - The string into which the substring will be copied.
 *
 * @return Pointer to the destination string after the substring has been copied to it.
 *
 * NOTE: If from, to aren't from a single string, or if to >= from behaviour is undefined.
 *       dest must be allocated correctly before calling the function, else behaviour is
 *       undefined.
 */

char *CopySubstring(const char *from, const char *to, char *dest);

/*
 * @brief Creates a deep copy of a string.
 *
 * @param str - The string to duplicate.
 *
 * @return Upon succes, a pointer to a newly allocated string. 
 *         Upon failure, NULL.
 */

char *StrDup(const char *str);

/* 
 * @brief A portable ANSI-supported implementation of 'isblank'.
 *
 * @param c - A character.
 *
 * @return 1 if c is a space or tab character, 0 otherwise.
 */
int IsBlank(int c);

#endif /* __STRING_UTILS__ */
