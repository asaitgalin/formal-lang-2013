#ifndef _LOLCODE_STMT_H_
#define _LOLCODE_STMT_H_

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "lolcode_value.h"
#include "lolcode_type.h"
#include "lolcode_utils.h"

extern int yylineno;

/* ===== Interfaces ===== */

class Expr {
public:
    virtual Value *eval() = 0;
};

class Stmt {
public:
    virtual void execute() = 0;
};

class StmtList {
public:
    void execute() const;
    void add(Stmt *stmt);
   
    // Local variables table
    void addLocalVariable(const std::string &name, Value *val);
    Value *getLocalVariable(const std::string &name);

    // Temp variable IT
    void setTempValue(Value *tmp) { temp_ = tmp; }
    Value *getTempValue() { return temp_; }

private:
    std::unordered_map<std::string, Value *> variables_;
    std::vector<Stmt *> stmtList_;
    Value *temp_;
};

extern StmtList *program;

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

    virtual void execute() {
        if (expr_ == nullptr) {
            program->addLocalVariable(name_, new UntypedValue());
        } else {
            program->addLocalVariable(name_, expr_->eval());
        }
    }

private:
    std::string name_;
    Expr *expr_;
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

class StmtPrint: public Stmt {
public:

    StmtPrint(ExprList *list, bool needNewline):
        list_(list),
        needNewline_(needNewline)
    { }

    virtual void execute() {
        for (size_t i = 0; i < list_->getExprCount(); ++i) {
            Expr *expr = list_->getExpr(i);
            std::cout << expr->eval()->toString();
        }
        if (needNewline_) {
            std::cout << std::endl;
        }
    }

private:
    ExprList *list_;
    bool needNewline_;
};

class StmtGetLine: public Stmt {
public:
    
    StmtGetLine(const char *variable):
        variable_(variable)
    { }

    virtual void execute() {
        std::string input;
        std::cin >> input;
        program->addLocalVariable(variable_, new StringValue(input));
    }

private:
    std::string variable_;
};

class StmtBareExpr: public Stmt {
public:
    
    StmtBareExpr(Expr *expr):
        expr_(expr)
    { }

    virtual void execute() {
        program->setTempValue(expr_->eval());
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

    virtual void execute() {
        Value *castResult = castValue(type_, program->getLocalVariable(name_));
        program->addLocalVariable(name_, castResult);
    }

private:
    std::string name_;
    Type *type_;
};

/* ===== Expressions ===== */ 

class ExprVariable: public Expr {
public:
   
    ExprVariable(const char *name):
        name_(name)
    { }

    virtual Value *eval() {
        Value *var = program->getLocalVariable(name_);
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

    virtual Value *eval() {
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

    virtual Value *eval();

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

    virtual Value *eval();

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

    virtual Value *eval() {
        bool result = list_->getExpr(0);
        for (size_t i = 1; i < list_->getExprCount(); ++i) {
            if (op_ == '&') {
                result = result && list_->getExpr(i)->eval()->toBoolean();
            } else {
                result = result || list_->getExpr(i)->eval()->toBoolean();
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

    virtual Value *eval() {
        std::string result;
        for (size_t i = 0; i < list_->getExprCount(); ++i) {
            result.append(list_->getExpr(i)->eval()->toString());
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

    virtual Value *eval() {
        return castValue(type_, expr_->eval());
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

    virtual Value *eval();

private:
    bool isNumeric(Value *val);
    
    Expr *lhs_;
    Expr *rhs_;
    char op_;
};

class ExprTemporary: public Expr {
public:
    
    virtual Value *eval() {
        return program->getTempValue();
    }

};

#include "lolcode.tab.h"

// Forward declarations
int yylex();
void yyerror(std::string error);

#endif /* _LOLCODE_STMT_H_ */
