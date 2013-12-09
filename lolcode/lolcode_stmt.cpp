#include <iostream>
#include <sstream>
#include <cmath> 
#include <cfloat>
#include <regex>
#include <cctype>

#include "lolcode_stmt.h"
#include "lolcode_utils.h"

/* CodeBlock */

void CodeBlock::setLocalVariable(const std::string &name, Value *val) {
    auto it = variables_.find(name);
    if  (it == variables_.end()) {
        if (parent_ == NULL) {
            raiseMachineError("cannot set undeclared variable: \"" + name + "\"");
        } else {
            parent_->setLocalVariable(name, val);
        }
    } else {
        variables_[name] = val;
    }
}

Value *CodeBlock::getLocalVariable(const std::string &name) {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        if (parent_ == NULL) {
            raiseMachineError("use of unreferenced variable: \"" + name + "\"");
        } else {
            return parent_->getLocalVariable(name);
        }
    }
    return it->second;
}

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

bool validVariableChar(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || isdigit(c) || c == '_';
}

std::string StmtPrint::transformString(const std::string &s, CodeBlock *block) {
    std::string result = stringReplace(s, ":\"", "\"");
    result = stringReplace(result, ":)", "\n");
    result = stringReplace(result, ":>", "\t");
    std::string varName;
    std::string out;
    int flag = 0;
    for (auto it = result.cbegin(); it != result.cend(); ++it) {
        if (*it == ':' && flag == 0) {
            ++flag;
            continue;
        } 
        if (*it == ':' && flag == 1) {
            out += *it;
            flag = 0;
            continue;
        }
        if (*it == '{' && flag == 1) {
            ++flag;
            continue;
        }
        if (*it == '}' && flag == 2) {
            if (!varName.empty()) {
                out += block->getLocalVariable(varName)->toString();
            }
            flag = 0;
            varName.clear();
            continue;
        }
        if (flag == 2) {
            if (varName.empty() && isdigit(*it)) {
                raiseMachineError("pattern error in formatted output");
            }
            if (!validVariableChar(*it)) {
                raiseMachineError("pattern error in formatted output");
            }
            varName += *it;
            continue;
        }
        if (flag == 0) {
            out += *it;
            continue;
        }
        raiseMachineError("pattern error in formatted output");
    }
    return out;
}

/* StmtCycle */

stmtResult_t StmtCycle::execute(CodeBlock *block) {
    if (label_ != endLabel_) {
        raiseMachineError("cycle label \"" + label_ + "\" does not match \"" + endLabel_ + "\"");
    }
    CodeBlock *innerBlock = new CodeBlock(block, BT_CYCLE);
    bool stopped = false;
    while (!stopped) {
        for (auto it = stmts_->stmtList_.cbegin(); it != stmts_->stmtList_.cend(); ++it) {
            stmtResult_t result = (*it)->execute(innerBlock);
            if (result == SR_BREAK) {
                stopped = true;
                break;
            } else if (result == SR_RETURN) {
                raiseMachineError("cannot do FOUND YR from cycle");
            }
        }
    }
    return SR_NO_RETURN;
}

/* ExprFunctionCall */

Value *ExprFunctionCall::eval(CodeBlock *block) {
    program->setLastReturn(NULL);
    auto function = program->getFunction(name_);
    auto signature = function.first->getArguments();
    if (list_->getExprCount() != signature.size()) {
        raiseMachineError("function call does not match signature of \"" + name_ + "\"");
    }
    CodeBlock *innerBlock = new CodeBlock(NULL, BT_FUNCTION);
    for (size_t i = 0; i < signature.size(); ++i) {
        innerBlock->declareVariable(signature[i], list_->getExpr(i)->eval(block));
    }
    auto stmts = function.second;
    for (auto it = stmts->stmtList_.cbegin(); it != stmts->stmtList_.cend(); ++it) {
        stmtResult_t result = (*it)->execute(innerBlock);
        if (result == SR_BREAK) {
            return new UntypedValue();
        } else if (result == SR_RETURN) {
            return program->getLastReturn();
        }
    }
    return innerBlock->getTempValue();
}

