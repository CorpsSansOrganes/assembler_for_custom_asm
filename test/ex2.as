  
  macr cmpmacro
  cmp r2, *r1
  endmacr

  ;Using macro
  cmpmacro
  symbol1: .data +3,  -13,  5
  .string "ab cd"
  .entry symbol2,    symbol1

  inc r6
  bne *r4
  symbol2: mov #4,   r3
  cmp symbol3, *r2
  
  ; Another comment
  symbol5: .extern symbol3
  stop
