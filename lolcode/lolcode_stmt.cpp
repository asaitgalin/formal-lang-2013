#include <iostream>
#include <sstream>
#include <cmath>
#include <cfloat>
#include <regex>

#include "lolcode_stmt.h"
#include "lolcode_utils.h"

/* StmtList */

stmtResult_t StmtList::execute(CodeBlock *block) const {
    for (auto it = stmtList_.cbegin(); it != stmtList_.cend(); ++it) {
        stmtResult_t result = (*it)->execute(block);
        if (result != SR_NO_RETURN) {
            return result;
        }
    }
    return SR_NO_RETURN;
}

void StmtList::add(Stmt *stmt) {
    stmtList_.push_back(stmt);
}

/* ExprArithm */

Value *ExprArithm::eval(CodeBlock *block) {
    Value *left = lhs_->eval(block);
    Value *right = rhs_->eval(block);
    // Type conversion and casts
    NumericCastResult lhsCast = castToNumeric(left);
    NumericCastResult rhsCast = castToNumeric(right);
    NumericCastResult result;
    result.type = Type::getMaxType(lhsCast.type, rhsCast.type);
    lhsCast.convertToMaxType(result.type);
    rhsCast.convertToMaxType(result.type);
    // Arithmetic
    if (result.type == Type::_integer) {
        if (op_ == '%') {
            result.intVal = lhsCast.intVal * rhsCast.intVal;
        } else {
            result.intVal = ExprProcessor::processArithmetic<int>(lhsCast.intVal, rhsCast.intVal, op_);
        }
    } else {
        if (op_ == '%') {
            raiseMachineError("mod is not allowed for floating point expressions");
        }
        result.floatVal = ExprProcessor::processArithmetic<float>(lhsCast.floatVal, rhsCast.floatVal, op_);
    }
    if (result.type == Type::_float) {
        return new FloatValue(result.floatVal);
    }
    return new IntValue(result.intVal);
}

/* ExprLogical */

Value *ExprLogical::eval(CodeBlock *block) {
    bool lhsBool = lhs_->eval(block)->toBoolean();
    bool rhsBool;
    if (rhs_) {
        rhsBool = rhs_->eval(block)->toBoolean();
    }
    Value *result;
    switch (op_) {
        case '&':
            result = new BoolValue(lhsBool && rhsBool);
            break;
        case '|':
            result = new BoolValue(lhsBool || rhsBool);
            break;
        case '^':
            result = new BoolValue(lhsBool != rhsBool);
            break;
        case '!':
            result = new BoolValue(!lhsBool);
            break;
    }
    return result;
}

/* ExprComparison */

bool ExprComparison::isNumeric(Value *val) {
    return val->getType() == Type::_integer || val->getType() == Type::_float;
}

Value *ExprComparison::eval(CodeBlock *block) {
    Value *left = lhs_->eval(block);
    Value *right = rhs_->eval(block);
    bool result;
    if (left->getType() == Type::_string && right->getType() == Type::_string) {
        result = left->toString() == right->toString();
    } else if (isNumeric(left) && isNumeric(right)) {
            // Type conversion and casts (elevation)
            Type *maxType = Type::getMaxType(left->getType(), right->getType());
            // Comparison
            if (maxType == Type::_float) {
                bool res;
                if (op_ == '=') {
                    result = fabs(left->toFloat(res) - right->toFloat(res)) < FLT_EPSILON;
                } else {
                    result = fabs(left->toFloat(res) - right->toFloat(res)) > FLT_EPSILON;
                }
            } else {
                bool res;
                if (op_ == '=') {
                    result = left->toInteger(res) == right->toInteger(res);
                } else {
                    result = left->toInteger(res) != right->toInteger(res);
                }
            }
    } else if (left->getType() == Type::_boolean && right->getType() == Type::_boolean) {
        result = left->toBoolean() == right->toBoolean(); 
    } else {
        raiseMachineError("comparison failed: arguments have invalid types");
    }
    return new BoolValue(result);
}

/* StmtPrint */

std::string StmtPrint::stringReplace(const std::string &s, const std::string &search, const std::string &replace) { 
    std::string subject(s);
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

std::string StmtPrint::transformString(const std::string &s, CodeBlock *block) {
    std::string result = stringReplace(s, "::", ":");
    result = stringReplace(result, ":\"", "\"");
    result = stringReplace(result, ":)", "\n");
    result = stringReplace(result, ":>", "\t");
    //
    return result;
}

