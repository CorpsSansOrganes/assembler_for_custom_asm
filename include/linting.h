#ifndef __SH_ED_LINTING__
#define __SH_ED_LINTING__

#include "utils.h"

/*
 * @brief This function removes any extrenuous whitespaces.
 *
 *        The function receives a line of text terminating with '\n', in
 *        the form of a string.
 *        The line is then modified in the following ways:
 *        
 *        1. Leading & trailing whitespaces are removed.
 *        2. Any numeruous continuous whitespaces are replaced w/ only one.
 *
 *        We assume that the line isn't blank.
 *
 * @param line - A line of text, terminated by '\n'.
 *
 * @return Pointer to the beginning of the clean line.
 */

char *CleanLine(char *line);

/*
 * @brief Checks if a line is blank (containing only white spaces, or no
 *        characters at all).
 * 
 * @param line - Pointer to a null-terminated string containing the line to be
 *               checked.
 *               We assume that StripWhitespaces has been called on line.
 * 
 * @return TRUE if the line is blank, FALSE otherwise.
 */

bool_t IsBlankLine(const char *line);

#endif /* __SH_ED_LINTING__ */

