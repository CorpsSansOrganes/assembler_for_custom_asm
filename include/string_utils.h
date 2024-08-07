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

#endif /* __STRING_UTILS__ */
