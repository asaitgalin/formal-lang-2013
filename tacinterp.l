%option noyywrap
%{
#include <stdio.h>
#include <stdlib.h>

#include "variable_table.h"

#define MAX_ARGCOUNT 3

size_t lineCount;
hashTable_t table;

enum commands_t {
    C_LET = 0,
    C_MOV, 
    C_ADD, C_SUB, C_MUL, C_DIV,
    C_JMP, C_CMP, // to be implemented
    C_OUT
};

enum argType_t {
    AT_NUMBER = 0,
    AT_VARIABLE
};

typedef struct {
    int argType;
    char *name;
    int value;
} argument_t;

typedef struct {
    size_t needCount;
    size_t currentCount;
    int currentCmd;
} lexerState_t;

lexerState_t state;
argument_t arguments[MAX_ARGCOUNT];

void updateState(int newCmd, size_t newNeedCount) {
    state.currentCmd = newCmd;
    state.needCount = newNeedCount;
    state.currentCount = 0;
}

void processCmd() {
    switch (state.currentCmd) {
        case C_LET:
            if (arguments[0].argType != AT_VARIABLE || arguments[1].argType != AT_NUMBER) {
                fprintf(stderr, "Command let syntax error\n, line: %d", lineCount);
                exit(-1);
            }
            hashTable_setValue(&table, arguments[0].name, arguments[1].value);
            free(arguments[0].name);
            break;
        case C_OUT:
            if (arguments[0].argType != AT_VARIABLE) {
                fprintf(stderr, "Command out syntax error\n, line: %d", lineCount);
                exit(-1);
            }
            int value;
            hashTable_getValue(&table, arguments[0].name, &value);
            printf("%d\n", value);
            free(arguments[0].name);
            break;
        case C_ADD:
        case C_SUB:
        case C_MUL:
        case C_DIV:
            if (arguments[2].argType != AT_VARIABLE) {
                fprintf(stderr, "Command add syntax error\n, line: %d", lineCount);
                exit(-1);
            }
            int firstVal, secondVal;
            // Process first arg
            if (arguments[0].argType == AT_NUMBER) {
                firstVal = arguments[0].value;
            } else {
                hashTable_getValue(&table, arguments[0].name, &firstVal);
                free(arguments[0].name);
            }
            // Process second arg
            if (arguments[1].argType == AT_NUMBER) {
                secondVal = arguments[1].value;
            } else {
                hashTable_getValue(&table, arguments[1].name, &secondVal);
                free(arguments[1].name);
            }
            int resultVal = firstVal + secondVal;
            if (state.currentCmd == C_SUB) {
                resultVal = firstVal - secondVal;
            } else if (state.currentCmd == C_MUL) {
                resultVal = firstVal * secondVal;
            } else if (state.currentCmd == C_DIV) {
                resultVal = firstVal / secondVal;
            }
            hashTable_setValue(&table, arguments[2].name, resultVal);
            free(arguments[2].name);
            break;
    }
}

%}

NUMBER      [0-9]+
VARIABLE    [a-zA-Z_][a-zA-Z0-9_]*  
SPACE       [ \t]+

%%

let                    { updateState(C_LET, 2);   }
mov                    { updateState(C_MOV, 2);   }
add                    { updateState(C_ADD, 3);   }
sub                    { updateState(C_SUB, 3);   }
mul                    { updateState(C_MUL, 3);   }
div                    { updateState(C_DIV, 3);   }
jmp                    { printf("Jmp command\n"); }
cmp                    { printf("Cmp command\n"); }
out                    { updateState(C_OUT, 1);   }

{NUMBER}               {    
                            int index = state.currentCount;
                            arguments[index].argType = AT_NUMBER;
                            arguments[index].value = atoi(yytext);
                            ++state.currentCount;
                            if (state.currentCount == state.needCount) {
                                processCmd();
                            }
                       }

{VARIABLE}             {   
                            int index = state.currentCount;
                            arguments[index].argType = AT_VARIABLE;
                            arguments[index].name = (char *)calloc(strlen(yytext) + 1, sizeof(char));
                            strcpy(arguments[index].name, yytext);
                            ++state.currentCount;
                            if (state.currentCount == state.needCount) {
                                processCmd();
                            }
                       }

{SPACE}                // skip all spaces

\n                     { ++lineCount; }
.                      { printf("Syntax error, line: %d", lineCount); }

%%

int main(int argc, char *argv[]) {
    if (argc < 2) {
        yyin = stdin;
    } else {
        yyin = fopen(argv[1], "r");
    }
    lineCount = 0;
    hashTable_init(7, &table);
    yylex();	
	hashTable_clear(&table);
    return 0;
}

