#include <iostream>
#include <sstream>
#include <cmath>
#include <cfloat>

#include "lolcode_stmt.h"
#include "lolcode_utils.h"

/* StmtList */

void StmtList::execute() const {
    for (auto it = stmtList_.cbegin(); it != stmtList_.cend(); ++it) {
        (*it)->execute();
    }
}

void StmtList::add(Stmt *stmt) {
    stmtList_.push_back(stmt);
}

void StmtList::addLocalVariable(const std::string &name, Value *val) {
    variables_[name] = val;
}

Value *StmtList::getLocalVariable(const std::string &name) {
    auto it = variables_.find(name);
    if (it == variables_.cend()) {
        return nullptr;
    }
    return it->second;
}

/* ExprArithm */

Value *ExprArithm::eval() {
    Value *left = lhs_->eval();
    Value *right = rhs_->eval();
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

Value *ExprLogical::eval() {
    bool lhsBool = lhs_->eval()->toBoolean();
    bool rhsBool;
    if (rhs_) {
        rhsBool = rhs_->eval()->toBoolean();
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

Value *ExprComparison::eval() {
    Value *left = lhs_->eval();
    Value *right = rhs_->eval();
    if (!isNumeric(left) || !isNumeric(right)) {
        raiseMachineError("comparison arguments are not numeric");
    }
    // Type conversion and casts (elevation)
    Type *maxType = Type::getMaxType(left->getType(), right->getType());
    bool result;
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
    return new BoolValue(result);
}

