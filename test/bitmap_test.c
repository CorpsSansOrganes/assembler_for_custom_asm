#include <string.h> /* strcmp */
#include <stdio.h> /* printf */
#include "bitmap.h"
#include "test_utils.h"

test_info_t BitmapTest(void) {
  test_info_t test_info = InitTestInfo("BitmapTest");
  bitmap_t map = 0;
  size_t i = 0;
  char str[WORD_SIZE];

  /* Sanity check */
  for (i = 0; i < WORD_SIZE; ++i) {
    if (0 != GetBitValue(map, i)) {
      RETURN_ERROR(TEST_FAILED);
    }
  }

  /* Set values, check we got what we expected */
  map = SetBitOn(map, 0);
  if (0b000000000000001 != map) {
      RETURN_ERROR(TEST_FAILED);
  }

  map = SetBitOn(map, 3);
  if (0b000000000001001 != map) {
    RETURN_ERROR(TEST_FAILED);
  }

  map = SetBitOn(map, WORD_SIZE - 1);
  if (0b100000000001001 != map) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* Turn off bit */
  map = SetBitOff(map, 3);
  if (0b100000000000001 != map) {
    RETURN_ERROR(TEST_FAILED);
  }

  /* Test BitmapToString */
  if (strcmp("100000000000001", BitmapToString(map, str))) {
    RETURN_ERROR(TEST_FAILED);
  }

  return test_info;
}

int main(void) {
  if (TEST_SUCCESSFUL == BitmapTest().result) {
    printf("Test successful: Bitmap\n");
  }

  return 0;
}
