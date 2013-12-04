#ifndef _LOLCODE_UTILS_H_
#define _LOLCODE_UTILS_H_

#include "lolcode_value.h"
#include "lolcode_type.h"

struct NumericCastResult {
    Type *type;
    
    union {
        int intVal; 
        float floatVal;
    };

    void convertToMaxType(Type *maxType) {
        if (type != maxType) {
            // Elevate int to float
            type = Type::_float;
            floatVal = intVal;
        }
    }

};

class ExprProcessor {
public:

    template<typename _Ty>
    static _Ty processArithmetic(_Ty lhs, _Ty rhs, char op) {
        _Ty result;
        switch (op) {
            case '+':
                result = lhs + rhs;
                break;
            case '-':
                result = lhs - rhs;
                break;
            case '*':
                result = lhs * rhs;
                break;
            case '/':
                result = lhs / rhs;
                break;
            // case '%':
            case 'i':
                result = lhs > rhs ? lhs : rhs;
                break;
            case 'a':
                result = lhs < rhs ? lhs : rhs;
                break;
        }
        return result;
    }

};

Value *castValue(Type *targetType, Value *src);
void raiseMachineError(const std::string &error);
NumericCastResult castToNumeric(Value *value);

#endif /* _LOLCODE_UTILS_H_ */
