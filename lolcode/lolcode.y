%{
#include "lolcode_stmt.h"
#include "lolcode_type.h"
%}

%union {
    StmtList *stmtList;
    ElseIfBlockList *elseIfBlocks;
    FunctionSignature *signature;
    Stmt *stmt;
    Expr *constant;    
    Expr *term; Expr *expr;
    Stmt *cycleStmt;
    Type *exprType;
    ExprList *list;
    cycleType_t cycleType;
    char cycleOp;
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

/* IO */
%token VISIBLE GET_LINE

/* IT */
%token TEMP_VAR

/* Math operators */
%token ADDITION SUBTRACTION
%token MULTIPLICATION DIVISION MODULO
%token BIGGER SMALLER

/* Logical operators */
%token BOOL_AND
%token BOOL_OR
%token BOOL_XOR
%token BOOL_NOT
%token BOOL_INF_AND BOOL_INF_OR

/* Concatenation */
%token STR_CONCAT

/* Cast */
%token CAST
%token CAST_SEPARATOR
%token BOOL_TYPE STR_TYPE INT_TYPE FLOAT_TYPE 
%token UNTYPED_TYPE
%token VARIABLE_TYPE_CHANGE 

/* Comparison */
%token EQUALS
%token NOT_EQUALS

/* Flow */
%token IF_BEGIN IF_TRUE IF_ELSEIF IF_FALSE IF_END

/* Cycles */ 
%token CYCLE_BEGIN
%token SIGNATURE_SEPARATOR
%token CYCLE_INC CYCLE_DEC CYCLE_UNTIL CYCLE_WHILE
%token CYCLE_END

/* Functions */
%token FUNCTION_BEGIN FUNCTION_END
%token FUNCTION_RETURN_NULL FUNCTION_RETURN

/* Comments */
%token SL_COMMENT
%token ML_COMMENT_BEGIN ML_COMMENT_END

%token ARG_SEPARATOR
%token VISIBLE_FLAG // [!]

%type <stmtList> true_block
%type <elseIfBlocks> else_if_block_list
%type <stmtList> false_block

%type <stmtList> stmt_list
%type <stmt> stmt
%type <constant> constant
%type <term> term
%type <expr> expr
%type <expr> assign_expr
%type <list> expr_list
%type <list> expr_list_separator
%type <exprType> expr_type
%type <signature> func_arg_list
%type <signature> func_signature
%type <list> fc_expr_list
%type <cycleOp> cycle_op
%type <cycleType> cycle_type

%%

program
    : CODE_BEGIN newline stmt_list CODE_END newline { program = new Program($3); }
    ;

stmt_list
    : stmt_list stmt stmt_separator { if ($2 != NULL) { $1->add($2); } }
    | /* epsilon */ { $$ = new StmtList(); }
    ;

stmt_separator
    : ','
    | newline
    ;

newline
    : '\n'
    | newline '\n'
    ;

true_block
    : IF_TRUE stmt_separator stmt_list { $$ = $3; }
    ;

else_if_block_list
    : else_if_block_list IF_ELSEIF expr stmt_separator stmt_list { $$->addBlock($3, $5); } 
    | /* epsilon */ { $$ = new ElseIfBlockList(); }
    ;

false_block
    : IF_FALSE stmt_separator stmt_list { $$ = $3; }
    | /* epsilon */ { $$ = new StmtList(); }
    ;

func_arg_list
    : func_arg_list ARG_SEPARATOR SIGNATURE_SEPARATOR VARIABLE_ID { $$->addArgument($4); } 
    | SIGNATURE_SEPARATOR VARIABLE_ID { $$ = new FunctionSignature(); $$->addArgument($2); }

func_signature
    : func_arg_list { $$ = $1; } 
    | /* epsilon */ { $$ = new FunctionSignature(); }
    ;

stmt
    : VARIABLE_DECL VARIABLE_ID VARIABLE_INIT constant { $$ = new StmtVariableDecl($2, $4); }
    | VARIABLE_DECL VARIABLE_ID { $$ = new StmtVariableDecl($2); }
    | VISIBLE expr_list { $$ = new StmtPrint($2, true); }
    | VISIBLE expr_list VISIBLE_FLAG { $$ = new StmtPrint($2, false); }
    | GET_LINE VARIABLE_ID { $$ = new StmtGetLine($2); }
    | VARIABLE_ID VARIABLE_ASSIGN assign_expr { $$ = new StmtVariableDecl($1, $3); } | VARIABLE_ID VARIABLE_TYPE_CHANGE expr_type { $$ = new StmtVariableCast($1, $3); } | IF_BEGIN newline true_block else_if_block_list false_block IF_END { $$ = new StmtConditional($3, $4, $5); }
    | FUNCTION_BEGIN VARIABLE_ID func_signature '\n' stmt_list FUNCTION_END { $$ = new StmtFunction($2, $3, $5); } 
    | FUNCTION_RETURN_NULL { $$ = new StmtFunctionReturn(NULL); }
    | FUNCTION_RETURN expr { $$ = new StmtFunctionReturn($2); }
    | VARIABLE_ID fc_expr_list { $$ = new StmtBareExpr(new ExprFunctionCall($1, $2)); } 
    | SL_COMMENT { $$ = NULL; }
    | ML_COMMENT_BEGIN ML_COMMENT_END { $$ = NULL; }
    | CYCLE_BEGIN SIGNATURE_SEPARATOR VARIABLE_ID '\n' stmt_list CYCLE_END SIGNATURE_SEPARATOR VARIABLE_ID { $$ = new StmtCycle($3, $5, $8); } 
    | CYCLE_BEGIN SIGNATURE_SEPARATOR VARIABLE_ID cycle_op SIGNATURE_SEPARATOR VARIABLE_ID '\n' stmt_list CYCLE_END SIGNATURE_SEPARATOR VARIABLE_ID { $$ = new StmtCycle($3, $4, $6, CT_WHILE, NULL, $8, $11); }
    | CYCLE_BEGIN SIGNATURE_SEPARATOR VARIABLE_ID cycle_op SIGNATURE_SEPARATOR VARIABLE_ID cycle_type expr '\n' stmt_list CYCLE_END SIGNATURE_SEPARATOR VARIABLE_ID { $$ = new StmtCycle($3, $4, $6, $7, $8, $10, $13); }
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
    /* Comparison */
    | EQUALS expr arg_separator expr { $$ = new ExprComparison($2, $4, '='); }
    | NOT_EQUALS expr arg_separator expr { $$ = new ExprComparison($2, $4, '!'); }
    /* Values */
    | term { $$ = $1; } 
    ;

cycle_type
    : CYCLE_WHILE { $$ = CT_WHILE; }
    | CYCLE_UNTIL { $$ = CT_UNTIL; }
    ;

cycle_op
    : CYCLE_INC { $$ = '+'; }
    | CYCLE_DEC { $$ = '-'; }
    ;

fc_expr_list
    : fc_expr_list expr { $$->putExpr($2); }
    | expr { $$ = new ExprList(); $$->putExpr($1); }
    ;

assign_expr
    : expr { $$ = $1; } 
    | VARIABLE_ID fc_expr_list { $$ = new ExprFunctionCall($1, $2); }
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

