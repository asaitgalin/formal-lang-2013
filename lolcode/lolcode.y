%{
#include "lolcode_stmt.h"
#include "lolcode_type.h"
%}

%union {
    Stmt *stmt;
    Expr *constant;    
    Expr *term;
    Expr *expr;
    Type *exprType;
    ExprList *list;
    char varName[256];
    int intValue;
    float floatValue;
    char stringValue[256];
    bool boolValue;
};

%token CODE_BEGIN CODE_END
%token VARIABLE_DECL VARIABLE_INIT
%token VARIABLE_ASSIGN
%token <varName> VARIABLE_ID
%token <intValue> INT_NUMERAL
%token <floatValue> FLOAT_NUMERAL
%token <stringValue> STRING_LITERAL 
%token <boolValue> BOOL_VALUE
%token VISIBLE
%token GET_LINE

/* IT */
%token TEMP_VAR

/* Math operators */
%token ADDITION
%token SUBTRACTION
%token MULTIPLICATION
%token DIVISION
%token MODULO
%token BIGGER
%token SMALLER

/* Logical operators */
%token BOOL_AND
%token BOOL_OR
%token BOOL_XOR
%token BOOL_NOT
%token BOOL_INF_AND
%token BOOL_INF_OR

/* Concatenation */
%token STR_CONCAT

/* Cast */
%token CAST
%token CAST_SEPARATOR
%token BOOL_TYPE
%token STR_TYPE
%token INT_TYPE
%token FLOAT_TYPE
%token UNTYPED_TYPE
%token VARIABLE_TYPE_CHANGE 

/* Comparison */
%token EQUALS
%token NOT_EQUALS

%token ARG_SEPARATOR
%token VISIBLE_FLAG // [!]

%type <stmt> stmt
%type <constant> constant
%type <term> term
%type <expr> expr
%type <list> expr_list
%type <list> expr_list_separator
%type <exprType> expr_type

%%

program
    : CODE_BEGIN newline stmt_list CODE_END newline { }
    ;

stmt_list
    : stmt_list stmt newline { program->add($2); }
    | stmt_list stmt ',' { program->add($2); }
    | /* epsilon */
    ;

newline
    : '\n'
    | newline '\n'
    ;

stmt
    : VARIABLE_DECL VARIABLE_ID VARIABLE_INIT constant { $$ = new StmtVariableDecl($2, $4); }
    | VARIABLE_DECL VARIABLE_ID { $$ = new StmtVariableDecl($2); }
    | VISIBLE expr_list { $$ = new StmtPrint($2, true); }
    | VISIBLE expr_list VISIBLE_FLAG { $$ = new StmtPrint($2, false); }
    | GET_LINE VARIABLE_ID { $$ = new StmtGetLine($2); }
    | VARIABLE_ID VARIABLE_ASSIGN expr { $$ = new StmtVariableDecl($1, $3); }
    | VARIABLE_ID VARIABLE_TYPE_CHANGE expr_type { $$ = new StmtVariableCast($1, $3); }
    | expr { $$ = new StmtBareExpr($1); }
    ;

expr
    /* Math expressions */
    : ADDITION expr arg_separator expr { $$ = new ExprArithm($2, $4, '+'); }
    | SUBTRACTION expr arg_separator expr { $$ = new ExprArithm($2, $4, '-'); }
    | MULTIPLICATION expr arg_separator expr { $$ = new ExprArithm($2, $4, '*'); }
    | DIVISION expr arg_separator expr { $$ = new ExprArithm($2, $4, '/'); }
    | MODULO expr arg_separator expr { $$ = new ExprArithm($2, $4, '%'); }
    | BIGGER expr arg_separator expr { $$ = new ExprArithm($2, $4, 'i'); }
    | SMALLER expr arg_separator expr { $$ = new ExprArithm($2, $4, 'a'); }
    /* Logical expressions */
    | BOOL_AND expr arg_separator expr { $$ = new ExprLogical($2, $4, '&'); }
    | BOOL_OR expr arg_separator expr { $$ = new ExprLogical($2, $4, '|'); }
    | BOOL_XOR expr arg_separator expr { $$ = new ExprLogical($2, $4, '^'); }
    | BOOL_NOT expr { $$ = new ExprLogical($2, NULL, '!'); }
    | BOOL_INF_AND expr_list_separator '\n' { $$ = new ExprLogicalInf($2, '&'); }
    | BOOL_INF_OR expr_list_separator '\n' { $$ = new ExprLogicalInf($2, '|'); }
    /* Concatenation */
    | STR_CONCAT expr_list_separator '\n' { $$ = new ExprStringConcat($2); }
    | CAST expr cast_arg_separator expr_type { $$ = new ExprCast($2, $4); }  
    | term { $$ = $1; } 
    /* Comparison */
    | EQUALS expr arg_separator expr { $$ = new ExprComparison($2, $4, '='); }
    | NOT_EQUALS expr arg_separator expr { $$ = new ExprComparison($2, $4, '!'); }
    ;

cast_arg_separator
    : CAST_SEPARATOR
    | /* epsilon */
    ;

arg_separator
    : ARG_SEPARATOR
    | /* epsilon */
    ;

expr_list_separator
    : expr_list_separator arg_separator expr { $$->putExpr($3); }
    | expr { $$ = new ExprList(); $$->putExpr($1); }
    ;

expr_list
    : expr_list expr { $$->putExpr($2); }
    | /* epsilon */ { $$ = new ExprList(); } 
    ;

expr_type
    : BOOL_TYPE { $$ = Type::_boolean; }
    | STR_TYPE { $$ = Type::_string; }
    | INT_TYPE { $$ = Type::_integer; }
    | FLOAT_TYPE { $$ = Type::_float; }
    | UNTYPED_TYPE { $$ = Type::_untyped; }
    ;

term
    : constant { $$ = $1; }
    | VARIABLE_ID { $$ = new ExprVariable($1); }
    | TEMP_VAR { $$ = new ExprTemporary(); }
    ;

constant
    : INT_NUMERAL { $$ = new ExprConstant($1); }
    | FLOAT_NUMERAL { $$ = new ExprConstant($1); }
    | STRING_LITERAL { $$ = new ExprConstant($1); }
    | BOOL_VALUE { $$ = new ExprConstant($1); } 
    ;

%%
