;Editor- Lior Dvora
;Test number- 3
;Details- Basic assembly code without macros

MAIN: mov r3, LENGTH
LOOP: jmp L1
prn #48
L1: inc K
bne LOOP
END: stop
LENGTH: .data 6,-9,15
K: .data 22
STR: .string "abcdef"
