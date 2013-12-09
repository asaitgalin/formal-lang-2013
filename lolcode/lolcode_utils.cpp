#include "lolcode_utils.h"

Value *castValue(Type *targetType, Value *src) {
    Value *result;
    bool successful = true;
    if (targetType == Type::_untyped) {
        result = new UntypedValue();
    } else if (targetType == Type::_integer) {
        int intVal = src->toInteger(successful, false);
        if (successful) {
            result = new IntValue(intVal);
        }
    } else if (targetType == Type::_boolean) {
        result = new BoolValue(src->toBoolean());    
    } else if (targetType == Type::_string) {
        result = new StringValue(src->toString(false));
    } else if (targetType == Type::_float) {
        float floatVal = src->toFloat(successful, false);
        if (successful) {
            result = new FloatValue(floatVal);
        }
    } if (!successful) {
        raiseMachineError("casting failed");
    }
    return result;
}

void raiseMachineError(const std::string &error) {
    std::cerr << std::endl << "MachineError: " << error << std::endl;
    exit(-1);
}

NumericCastResult castToNumeric(Value *value) {
    NumericCastResult result;
    bool isInt;
    result.intVal = value->toInteger(isInt);
    if (!isInt) {
        // Now try to cast to float
        bool isFloat;
        result.floatVal = value->toFloat(isFloat);
        if (!isFloat) {
            raiseMachineError("failed to cast value \"" + value->toString() + "\"to numeric type");
        }
        result.type = Type::_float;
    } else {
        result.type = Type::_integer;
    }
    return result;
}

