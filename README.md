formal-lang-2013
================

Homeworks for theory of formal languages. 

1. three-address-code
Three address code interpreter. Uses flex. Supported commands:
let arg1 K              | K is integer, arg0 := K
mov arg1 arg2           | arg2 := arg1
add arg1 arg2 arg3      | arg3 := arg1 + arg2
sub arg1 arg2 arg3      | arg3 := arg1 - arg2
mul arg1 arg2 arg3      | arg3 := arg1 * arg2
div arg1 arg2 arg3      | arg3 := arg1 / arg2
jmp N                   | jump to line N
cmp arg1 arg2 Nb Ne Na  | jump to line Nb if arg1 < arg2, line Ne if arg1 = arg2, line Na if arg1 > arg2
out arg1                | print variable

All variables are considered as already declared.

2. lolcode
LOLCODE interpreter. Uses flex and bison.

Will be here soon :)
