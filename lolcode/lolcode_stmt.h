#ifndef _LOLCODE_STMT_H_
#define _LOLCODE_STMT_H_

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <list>

#include "lolcode_utils.h"
#include "lolcode_value.h"
#include "lolcode_type.h"

extern int yylineno;

enum stmtResult_t {
    SR_NO_RETURN = 0,
    SR_BREAK,
    SR_RETURN
};

enum blockType_t {
    BT_MAIN_FLOW = 0,
    BT_CYCLE,
    BT_FUNCTION
};

class CodeBlock {
public:

    CodeBlock(CodeBlock *parent, blockType_t type):
        parent_(parent),
        type_(type)
    { }
    
    void setLocalVariable(const std::string &name, Value *val);

    void declareVariable(const std::string &name, Value *initVal) {
        variables_[name] = initVal;
    }

    Value *getLocalVariable(const std::string &name);

    // Temp variable IT
    void setTempValue(Value *tmp) { temp_ = tmp; }
    Value *getTempValue() { return temp_; }

    // Type
    blockType_t getType() const {
        return type_;
    }

    CodeBlock *getParent() {
        return parent_;
    }

private:
    std::unordered_map<std::string, Value *> variables_;
    CodeBlock *parent_;
    blockType_t type_;
    Value *temp_;
};

/* ===== Interfaces ===== */

class Expr {
public:
    virtual Value *eval(CodeBlock *block) = 0;
};

class Stmt {
public:
    virtual stmtResult_t execute(CodeBlock *block) = 0;
};

/* ===== Helper classes ===== */

class StmtList {
public:
    stmtResult_t execute(CodeBlock *block) const;
    void add(Stmt *stmt);
    std::vector<Stmt *> stmtList_;
};

class FunctionSignature {
public:
    
    void addArgument(const char *name) {
        args_.push_back(std::string(name));
    }

    const std::vector<std::string> &getArguments() const {
        return args_;
    }

private:
    std::vector<std::string> args_;
};

class ElseIfBlockList {
public:
    
    void addBlock(Expr *cond, StmtList *actions) {
        blocks_.push_back(std::make_pair(cond, actions));
    }

    std::pair<Expr *, StmtList *> getBlock(size_t index) {
        return blocks_.at(index);
    }

    size_t getBlockCount() {
        return blocks_.size();
    }

private:
    std::vector<std::pair<Expr *, StmtList *>> blocks_;
};

class ExprList {
public:
    
    void putExpr(Expr *expr) {
        exprs_.push_back(expr);
    }
    
    Expr *getExpr(int index) {
        return exprs_.at(index);
    }

    size_t getExprCount() {
        return exprs_.size();
    }

private:
    std::vector<Expr *> exprs_;
};

/* Main program class */

class Program {
public:
   
    Program(StmtList *list) {
        mainBlock_ = new CodeBlock(NULL, BT_MAIN_FLOW);
        list_ = list;
    }

    CodeBlock *getMainBlock() {
        return mainBlock_;
    }

    void run() {
        list_->execute(mainBlock_);
    }
    
    // Functions
    void addFunction(const std::string &name, FunctionSignature *signature, StmtList *stmts) {
        if (functions_.find(name) != functions_.cend()) {
            raiseMachineError("function \"" + name + "\" is already declared");
        }
        functions_[name] = std::make_pair(signature, stmts);
    }

    std::pair<FunctionSignature *, StmtList  *> getFunction(const std::string &name) {
        return functions_[name];
    }

    bool hasFunction(const std::string &name) {
        return functions_.find(name) != functions_.cend();
    }

    // Return values
    Value *getLastReturn() {
        return lastReturn_;
    }

    void setLastReturn(Value *ret) {
        lastReturn_ = ret;
    }

private:
    std::unordered_map<std::string, std::pair<FunctionSignature *, StmtList *>> functions_;
    StmtList *list_;
    CodeBlock *mainBlock_;
    Value *lastReturn_;
};

extern Program *program;

/* ===== Statements ===== */

class StmtVariableDecl: public Stmt {
public:
   
    StmtVariableDecl(const char *name, Expr *expr):
        name_(name), 
        expr_(expr)
    { }

    StmtVariableDecl(const char *name) {
        name_ = name;
        expr_ = nullptr;
    }

    virtual stmtResult_t execute(CodeBlock *block) {
        if (expr_ == nullptr) {
            block->declareVariable(name_, new UntypedValue());
        } else {
            block->declareVariable(name_, expr_->eval(block));
        }
        return SR_NO_RETURN;
    }

private:
    std::string name_;
    Expr *expr_;
};

class StmtPrint: public Stmt {
public:

    StmtPrint(ExprList *list, bool needNewline):
        list_(list),
        needNewline_(needNewline)
    { }

    virtual stmtResult_t execute(CodeBlock *block) {
        for (size_t i = 0; i < list_->getExprCount(); ++i) {
            Expr *expr = list_->getExpr(i); 
            std::string toPrint = expr->eval(block)->toString();
            std::cout << transformString(toPrint, block);
        }
        if (needNewline_) {
            std::cout << std::endl;
        }
        return SR_NO_RETURN;
    }

private:

    std::string stringReplace(const std::string &, const std::string &, const std::string &);
    std::string transformString(const std::string &, CodeBlock *);
    
    ExprList *list_;
    bool needNewline_;
};

class StmtGetLine: public Stmt {
public:
    
    StmtGetLine(const char *variable):
        variable_(variable)
    { }

    virtual stmtResult_t execute(CodeBlock *block) {
        std::string input;
        std::cin >> input;
        block->setLocalVariable(variable_, new StringValue(input));
        return SR_NO_RETURN;
    }

private:
    std::string variable_;
};

class StmtBareExpr: public Stmt {
public:
    
    StmtBareExpr(Expr *expr):
        expr_(expr)
    { }

    virtual stmtResult_t execute(CodeBlock *block) {
        block->setTempValue(expr_->eval(block));
        return SR_NO_RETURN;
    }

private:
    Expr *expr_;
};

class StmtVariableCast: public Stmt {
public:
    
    StmtVariableCast(const char *name, Type *type):
        name_(name),
        type_(type)
    { }

    virtual stmtResult_t execute(CodeBlock *block) {
        Value *castResult = castValue(type_, block->getLocalVariable(name_));
        block->setLocalVariable(name_, castResult);
        return SR_NO_RETURN;
    }

private:
    std::string name_;
    Type *type_;
};

class StmtConditional: public Stmt {
public:

    StmtConditional(StmtList *trueStmts, ElseIfBlockList *elseIfBlocks, StmtList *falseStmts):
        trueStmts_(trueStmts),
        falseStmts_(falseStmts),
        elseIfBlocks_(elseIfBlocks)
    { }

    virtual stmtResult_t execute(CodeBlock *block) {
        if (block->getTempValue()->toBoolean()) {
            return trueStmts_->execute(block);
        }
        for (size_t i = 0; i < elseIfBlocks_->getBlockCount(); ++i) {
            auto p = elseIfBlocks_->getBlock(i);
            if (p.first->eval(block)->toBoolean()) {
                return p.second->execute(block);
            }
        }
        return falseStmts_->execute(block);
    }

private:
    StmtList *trueStmts_;
    StmtList *falseStmts_;
    ElseIfBlockList *elseIfBlocks_;
};

class StmtFunction: public Stmt {
public:
    
    StmtFunction(const char *name, FunctionSignature *signature, 
            StmtList *statements):
        name_(name),
        signature_(signature),
        statements_(statements)
    { }

    virtual stmtResult_t execute(CodeBlock *block) {
        if (block->getType() != BT_MAIN_FLOW) {
            raiseMachineError("functions can be declared only in main scope");
        }
        program->addFunction(name_, signature_, statements_);
        return SR_NO_RETURN;
    }

private:
    std::string name_;
    FunctionSignature *signature_;
    StmtList *statements_;
};

class StmtFunctionReturn: public Stmt {
public:

    StmtFunctionReturn(Expr *ret):
        ret_(ret)
    { }

    virtual stmtResult_t execute(CodeBlock *block) {
        if (block->getType() == BT_MAIN_FLOW) {
            raiseMachineError("cannot return from main scope");
        }
        if (ret_) {
            if (block->getType() == BT_FUNCTION) {
                program->setLastReturn(ret_->eval(block));
            }
            return SR_RETURN;
        } else {
            if (block->getType() == BT_FUNCTION) {
                program->setLastReturn(new UntypedValue());
            }
            return SR_BREAK;
        }
    }

private:
    Expr *ret_;
};

class StmtCycle: public Stmt {
public:
    
    StmtCycle(const char *label, StmtList *stmts, const char *endLabel):
        label_(label),
        stmts_(stmts),
        endLabel_(endLabel)
    { }

    virtual stmtResult_t execute(CodeBlock *block);

private:
    std::string label_;
    StmtList *stmts_;
    std::string endLabel_;
};

/* ===== Expressions ===== */ 

class ExprFunctionCall: public Expr {
public:
    
    ExprFunctionCall(const char *name, ExprList *list):
        name_(name),
        list_(list)
    { }

    virtual Value *eval(CodeBlock *block);

private:
    ExprList *list_;
    std::string name_;
};

class ExprVariable: public Expr {
public:
   
    ExprVariable(const char *name):
        name_(name)
    { }

    virtual Value *eval(CodeBlock *block) {
        if (program->hasFunction(name_)) {
            ExprFunctionCall *call = new ExprFunctionCall(name_.c_str(), new ExprList());
            return call->eval(block);
        }
        Value *var = block->getLocalVariable(name_);
        if (var == NULL) {
            raiseMachineError("reference to undefined variable: " + name_);
        }
        return var;
    }

private:
    std::string name_;
};

class ExprConstant: public Expr {
public:

    ExprConstant(const char *s) {
        value_ = new StringValue(s);
    }

    ExprConstant(int val) {
        value_  = new IntValue(val);
    }

    ExprConstant(float val) {
        value_ = new FloatValue(val);
    }

    ExprConstant(bool val) {
        value_ = new BoolValue(val);
    }

    virtual Value *eval(CodeBlock *block) {
        return value_;
    }

private:
    Value *value_;
};

class ExprArithm: public Expr {
public:
    
    ExprArithm(Expr *lhs, Expr *rhs, char op):
        op_(op),
        lhs_(lhs),
        rhs_(rhs)
    { }

    virtual Value *eval(CodeBlock *block);

private:
    char op_;
    Expr *lhs_;
    Expr *rhs_;
};

class ExprLogical: public Expr {
public:
    
    ExprLogical(Expr *lhs, Expr *rhs, char op):
        op_(op),
        lhs_(lhs),
        rhs_(rhs)
    { }

    virtual Value *eval(CodeBlock *block);

private:
    char op_;
    Expr *lhs_;
    Expr *rhs_;
};

class ExprLogicalInf: public Expr {
public:
    
    ExprLogicalInf(ExprList *list, char op):
        list_(list),
        op_(op)
    { }

    virtual Value *eval(CodeBlock *block) {
        bool result = list_->getExpr(0);
        for (size_t i = 1; i < list_->getExprCount(); ++i) {
            if (op_ == '&') {
                result = result && list_->getExpr(i)->eval(block)->toBoolean();
            } else {
                result = result || list_->getExpr(i)->eval(block)->toBoolean();
            }
        }
        return new BoolValue(result);
    }

private:
    ExprList *list_;
    char op_;
};

class ExprStringConcat: public Expr {
public:

    ExprStringConcat(ExprList *list):
        list_(list)
    { }

    virtual Value *eval(CodeBlock *block) {
        std::string result;
        for (size_t i = 0; i < list_->getExprCount(); ++i) {
            result.append(list_->getExpr(i)->eval(block)->toString());
        }
        return new StringValue(result);
    }

private:
    ExprList *list_;
};

class ExprCast: public Expr {
public:

    ExprCast(Expr *expr, Type *type):
        expr_(expr),
        type_(type)
    { }

    virtual Value *eval(CodeBlock *block) {
        return castValue(type_, expr_->eval(block));
    }

private:
    Expr *expr_;
    Type *type_;
};

class ExprComparison: public Expr {
public:
    
    ExprComparison(Expr *lhs, Expr *rhs, char op):
        lhs_(lhs),
        rhs_(rhs),
        op_(op)
    { }

    virtual Value *eval(CodeBlock *block);

private:
    bool isNumeric(Value *val);
    
    Expr *lhs_;
    Expr *rhs_;
    char op_;
};

class ExprTemporary: public Expr {
public:
    
    virtual Value *eval(CodeBlock *block) {
        return block->getTempValue();
    }

};

#include "lolcode.tab.h"

// Forward declarations
int yylex();
void yyerror(std::string error);

#endif /* _LOLCODE_STMT_H_ */
