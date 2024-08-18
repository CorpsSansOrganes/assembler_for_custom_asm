; should print 6 errors
; instruction doesnt exist
boolbool r3, r2
; wrong number of operands
stop r1, #5
; wrong number of operands
mov #7
; incorrect addressing method
lea *r1
; immeadiate operand too big
cmp #3451435434, r2
; symbol wasnt defined
inc symbol
