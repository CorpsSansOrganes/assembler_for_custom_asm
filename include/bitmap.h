#ifndef __SH_ED_BITMAP__
#define __SH_ED_BITMAP__

/*
 * An implementation for a bitmap.
 *
 * The bitmap's size is WORD_SIZE. Least significant bit is 0, and most
 * significant bit is WORD_SIZE - 1.
 * Any bit out of this range is considered invalid.
 */

#include <stddef.h> /* size_t */

#define WORD_SIZE (15) /* Or: (sizeof(size_t) * CHAR_BIT */

typedef size_t bitmap_t;

/*
 * @brief Sets the nth bit on.
 *
 * @param Bitmap - The bitmap to modify.
 *        n - The bit position to modify.
 * 
 * NOTE: behaviour is undefined if n is out of range (i.e. <= WORD_SIZE).
 *
 * @return The modified bitmap.
 */
bitmap_t SetBitOn(bitmap_t bitmap, size_t n);

/*
 * @brief Sets the nth bit off.
 *
 * @param Bitmap - The bitmap to modify.
 *        n - The bit position to modify.
 * 
 * NOTE: behaviour is undefined if n is out of range (i.e. <= WORD_SIZE).
 *
 * @return The modified bitmap.
 */
bitmap_t SetBitOff(bitmap_t bitmap, size_t n);

/*
 * @brief Get the value of the nth bit.
 *
 * @param Bitmap - The bitmap whose bit value we wish to inquire.
 *        n - The bit position to get.
 * 
 * NOTE: behaviour is undefined if n is out of range (i.e. <= WORD_SIZE).
 *
 * @return 1 if the nth bit is set, 0 if it is off.
 */
unsigned int GetBitValue(bitmap_t bitmap, size_t n);

/*
 * @brief Converts a bitmap into a string representation
 *
 * @param bitmap - The bitmap to convert.
 *        str - Pointer to the string where the representation will be stored.
 *              str's size must be at least WORD_SIZE characters.
 *
 * NOTE: behaviour is undefined if n is out of range (i.e. <= WORD_SIZE).
 *       behaviour is undefined if str is too small.
 *
 * @return A pointer to the string containing the bitmap representation.
 */
char *BitmapToString(bitmap_t bitmap, char *str);

#endif /* __SH_ED_BITMAP__ */
