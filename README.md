# formal-lang-2013

Homeworks for theory of formal languages. 

## three-address-code

Three address code interpreter. Uses flex. Supported commands:

1. let arg1 K               
*K is integer, arg0 := K*
2. mov arg1 arg2            
*arg2 := arg1*
3. add arg1 arg2 arg3       
*arg3 := arg1 + arg2*
4. sub arg1 arg2 arg3      
*arg3 := arg1 - arg2*
5. mul arg1 arg2 arg3    
*arg3 := arg1 * arg2*
6. div arg1 arg2 arg3      
*arg3 := arg1 / arg2*
7. jmp N                   
*jump to line N*
8. cmp arg1 arg2 Nb Ne Na             
*jump to line Nb if arg1 < arg2, Ne if arg1 = arg2, Na if arg1 > arg2*
9. out arg1                
*print arg1*

All variables are considered as already declared.

## lolcode

LOLCODE interpreter. Uses flex and bison.

Will be here soon :)
