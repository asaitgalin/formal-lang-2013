#ifndef _TYPES_H_
#define _TYPES_H_

enum commands_t {
    C_LET = 0,
    C_MOV, 
    C_ADD, C_SUB, C_MUL, C_DIV,
    C_JMP, C_CMP, 
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

#endif

