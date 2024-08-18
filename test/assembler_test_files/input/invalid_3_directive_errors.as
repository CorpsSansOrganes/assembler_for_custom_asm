; should print 8 errors

; data parameter too big
.data 4, -3, -123321321312

; IsIllegalDataParameter
.data -13, boolbool

; is illegal string 
.string "sdvsdv

; is illegal string 
.string f"fdfd"

; imprintable characters
.string ""

; extern symbol is illegal
.extern  symbol5, sym:bol1

; entry symbol is illegal
.entry  symbol6, sym:bol1

; directive doesnt exist
.boolbool "asxsf"
