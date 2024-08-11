#include "assembler.h"
#include "utils.h"
#include "syntax_errors.h"
#include "macro_table.h"
#include "symbol_table.h"
#include <stdio.h> /* perror */
#include <string.h> 
#include <stdlib.h>


typedef struct {
  /* 1-3 words */
  bitmap_t *opcode;
  size_t size;
} line_opcode_t;
