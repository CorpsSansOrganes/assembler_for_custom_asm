#ifndef __LINTING__
#define __LINTING__

/*
 * @brief Cosmetic touch-up of an input file.
 *        1. Remove comments.
 *        2. Remove blank lines.
 *        3. Remove extraneous spaces, tabs, etc.
 *
 *  The input file is modified with those changes.
 */
void LintFile(char *file_path);

#endif /* __LINTING__ */
