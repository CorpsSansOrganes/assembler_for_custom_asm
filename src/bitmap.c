/* bitmap.c
 * This module implements the 'struct bitmap' and provides 
 * utilities such as setting bits on and off, converting the 
 * bitmap to a string, and a 'getbit' function.
 */

#include "bitmap.h"

#define BIT_AT(N) ((bitmap_t)1 << (N))

bitmap_t SetBitOn(bitmap_t bitmap, size_t n) {
  return (bitmap | BIT_AT(n));
}

bitmap_t SetBitOff(bitmap_t bitmap, size_t n) {
  return (bitmap & ~BIT_AT(n));
}

unsigned int GetBitValue(bitmap_t bitmap, size_t n) {
  return (bitmap & BIT_AT(n)) ? 1 : 0; 
}

char *BitmapToString(bitmap_t bitmap, char *str) {
  size_t i = 0;
  for (i = 0; i < WORD_SIZE; ++i) {
    str[i] = GetBitValue(bitmap, i) ? '1' : '0';
  }

  return str;
}
