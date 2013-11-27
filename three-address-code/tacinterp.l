%option noyywrap
%{
#include <stdio.h>
#include <stdlib.h>

#include "variable_table.h"
#include "offset_array.h"
#include "types.h"

#define MAX_ARGCOUNT 5

size_t lineCount;
size_t currentLine;
offsetArray_t offsets;
hashTable_t table;

lexerState_t state;
argument_t arguments[MAX_ARGCOUNT];

void raiseError() {
    fprintf(stderr, "Command syntax error, line: %zd\n", currentLine);
    exit(-1);
}

void updateState(int newCmd, size_t newNeedCount) {
    if (state.currentCount != state.needCount) {
        raiseError();
    }
    state.currentCmd = newCmd;
    state.needCount = newNeedCount;
    state.currentCount = 0;
}

int extractOperand(int argNum) {
    if (arguments[argNum].argType == AT_VARIABLE) {
        int value;
        hashTable_getValue(&table, arguments[argNum].name, &value);
        free(arguments[argNum].name);
        return value;
    } else {
        return arguments[argNum].value;
    }
}

void jumpToLine(int line) {
    int fileOffset;
    if (offsetArray_get(&offsets, line, &fileOffset) != 0)  {
        fprintf(stderr, "Error: line index out of bounds, line: %zd\n", currentLine);
    }
    fseek(yyin, fileOffset, SEEK_SET);
    currentLine = line;
    yyrestart(yyin);
}

void processArithmetic() {
    if (arguments[2].argType != AT_VARIABLE) {
        raiseError();
    }
    int firstVal = extractOperand(0);
    int secondVal = extractOperand(1);

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
}

void processCondition() {
    if (arguments[2].argType != AT_NUMBER || arguments[3].argType != AT_NUMBER || arguments[4].argType != AT_NUMBER) {
        raiseError();
    }
    int firstVal = extractOperand(0);
    int secondVal = extractOperand(1);
    int lines[3];

    lines[0] = extractOperand(2);
    lines[1] = extractOperand(3);
    lines[2] = extractOperand(4);

    if (firstVal < secondVal) {
        jumpToLine(lines[0]);
    } else if (firstVal == secondVal) {
        jumpToLine(lines[1]);
    } else {
        jumpToLine(lines[2]);
    }
}

void processCmd() {
    switch (state.currentCmd) {
        case C_LET:
            if (arguments[0].argType != AT_VARIABLE || arguments[1].argType != AT_NUMBER) {
                raiseError();
            }
            hashTable_setValue(&table, arguments[0].name, arguments[1].value);
            free(arguments[0].name);
            break;
        case C_OUT:
            if (arguments[0].argType != AT_VARIABLE) {
                raiseError();
            }
            printf("%d\n", extractOperand(0));
            break;
        case C_MOV:
            if (arguments[0].argType != AT_VARIABLE || arguments[0].argType != AT_VARIABLE) {
                raiseError();
            }
            int value = extractOperand(0);
            hashTable_setValue(&table, arguments[1].name, value);
            break;
        case C_ADD:
        case C_SUB:
        case C_MUL:
        case C_DIV:
            processArithmetic();
            break;
        case C_JMP:
            if (arguments[0].argType != AT_NUMBER) {
                raiseError();
            }
            jumpToLine(arguments[0].value);
            break;
        case C_CMP:
            processCondition(); 
            break;
    }
}

%}

NUMBER      [0-9]+
VARIABLE    [a-zA-Z_][a-zA-Z0-9_]*  
SPACE       [ \t]+

%%

let                    {    updateState(C_LET, 2);   }
mov                    {    updateState(C_MOV, 2);   }
add                    {    updateState(C_ADD, 3);   }
sub                    {    updateState(C_SUB, 3);   }
mul                    {    updateState(C_MUL, 3);   }
div                    {    updateState(C_DIV, 3);   }
jmp                    {    updateState(C_JMP, 1);   }
cmp                    {    updateState(C_CMP, 5);   }
out                    {    updateState(C_OUT, 1);   }

{NUMBER}               {    
                            if (state.currentCmd == C_NONE) {
                                fprintf(stderr, "Syntax error, arguments without command, line: %zd\n", currentLine);
                                exit(-1);
                            }
                            int index = state.currentCount;
                            arguments[index].argType = AT_NUMBER;
                            arguments[index].value = atoi(yytext);
                            ++state.currentCount;
                            if (state.currentCount == state.needCount) {
                                processCmd();
                                state.currentCmd = C_NONE;
                            }
                       }

{VARIABLE}             {   
                            if (state.currentCmd == C_NONE) {
                                fprintf(stderr, "Syntax error, variable without command, line: %zd\n", currentLine);
                                exit(-1);
                            }
                            int index = state.currentCount;
                            arguments[index].argType = AT_VARIABLE;
                            arguments[index].name = (char *)calloc(yyleng + 1, sizeof(char));
                            strcpy(arguments[index].name, yytext);
                            ++state.currentCount;
                            if (state.currentCount == state.needCount) {
                                processCmd();
                                state.currentCmd = C_NONE;
                            }
                       }

{SPACE}                // Skip all spaces 

\n                     { ++currentLine; }

.                      { fprintf(stderr, "Syntax error near character: %s, line: %zd\n", yytext, currentLine); }

<<EOF>>                {    
                            if (state.currentCmd != C_NONE) {
                                fprintf(stderr, "Syntax error: unexpected end of file\n"); 
                            }
                            yyterminate();
                       }

%%

void getLinesInfo(FILE *f) {
    int c;
    int cursor = 0;
    int prevPos = 0;
    lineCount = 0;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            ++lineCount;
            offsetArray_put(&offsets, prevPos);
            prevPos = cursor;
        }
        ++cursor;
    }
    fseek(f, 0, SEEK_SET);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: too few arguments. Usage: tacinterp input.tac\n");
        exit(-1);
    } 
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "Error: input file not found\n");
        exit(-1);
    }
    offsetArray_init(10, &offsets);
    hashTable_init(7, &table);
    state.currentCmd = C_NONE;
    currentLine = 0;
    getLinesInfo(yyin);
    yylex();	
	hashTable_clear(&table);
    offsetArray_clear(&offsets);
    return 0;
}

