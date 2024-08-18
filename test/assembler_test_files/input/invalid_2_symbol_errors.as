; should print 7 errors
; symbol already defined
SYMBOL1: .data 3, 4
SYMBOL1: .string "asf"
; symbol already define as extern
.extern SYMBOL2
.entry SYMBOL2
; symbol already define as extern 
.entry SYMBOL3
.extern SYMBOL3
; symbol name is illegal
S:ymbol: .data 7 
S:m
; symbol used as macro
macr macroname
stop
endmacr
macroname: cmp #5, #4
; no definition for symbol
SYMBOL4:
