#include "lolcode_type.h"

/* Types */

Type * Type::_untyped = new Type(dtUntyped);
Type * Type::_boolean = new Type(dtBoolean);
Type * Type::_integer = new Type(dtInteger);
Type * Type::_float = new Type(dtFloat);
Type * Type::_string = new Type(dtString);

Type *Type::getMaxType(Type *lhs, Type *rhs) {
    if (lhs == Type::_float || rhs == Type::_float) {
        return Type::_float;
    }
    return Type::_integer;
}

