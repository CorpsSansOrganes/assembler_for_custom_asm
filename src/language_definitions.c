#include "language_definitions.h"
#include <string.h> /* strlen */
#include <ctype.h> /* isdigit */


instruction_t reserved_instructions [NUM_OF_INSTRUCTIONS] = {
  {"mov", 959},  /* 0b1110111111 */ 
  {"cmp", 1023}, /* 0b1111111111 */ 
  {"add", 959},  /* 0b1110111111 */ 
  {"sub", 959},  /* 0b1110111111 */ 
  {"lea", 907},  /* 0b1110001011 */ 
  {"clr", 898},  /* 0b1110000010 */ 
  {"not", 898},  /* 0b1110000010 */ 
  {"inc", 898},  /* 0b1110000010 */ 
  {"dec", 898},  /* 0b1110000010 */ 
  {"jmp", 386},  /* 0b0110000010 */ 
  {"bne", 386},  /* 0b0110000010 */ 
  {"red", 898},  /* 0b1110000010 */ 
  {"prn", 962},  /* 0b1111000010 */ 
  {"jsr", 386},  /* 0b0110000010 */ 
  {"rts", 0},    /* 0b0000000000 */ 
  {"stop", 0}   /* 0b0000000000 */  
};

char *reserved_directives[NUM_OF_DIRECTIVES] = {
  ".data",
  ".entry",
  ".string",
  ".extern"
};

char *register_names[NUM_OF_REGISTERS] = {
  "r0",
  "r1",
  "r2",
  "r3",
  "r4",
  "r5",
  "r6",
  "r7"
};
