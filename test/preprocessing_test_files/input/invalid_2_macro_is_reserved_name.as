; Should print 3 error - usage of reserved name.
macr mov
  dec K
endmacr

macr r2
  dec K
endmacr

macr .data
  dec K
endmacr 

K:      .data 31
