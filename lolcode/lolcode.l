%{
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "lolcode_stmt.h"

bool lineContinuation = false;

enum comment_t {
    COMMENT_DISABLED = 0, 
    COMMENT_SL,
    COMMENT_ML
};

comment_t comment = COMMENT_DISABLED;

#define RET(x) if (comment == COMMENT_DISABLED) { return x; }

%}

DIGIT [0-9]
IDENT [A-Za-z][A-Za-z0-9_]*
INTEGER ([1-9]{DIGIT}*)|0
FLOAT ({DIGIT}+[.]{DIGIT}*)|({DIGIT}*[.]{DIGIT}+) 
FLOAT_EXP ({INTEGER}|{FLOAT})[eE][+-]?{DIGIT}+ 
STRING \"([^:\"]|:.)*\" 
SPACES [ \t]+

%option noyywrap
%option yylineno
%%
HAI                       {  RET(CODE_BEGIN)  }
KTHXBYE                   {  RET(CODE_END)  }
I{SPACES}HAS{SPACES}A     {  RET(VARIABLE_DECL)  }
ITZ                       {  RET(VARIABLE_INIT)  }
R                         {  RET(VARIABLE_ASSIGN)  }

IT                        {  RET(TEMP_VAR)  }

SUM{SPACES}OF             {  RET(ADDITION)  }
DIFF{SPACES}OF            {  RET(SUBTRACTION)  }
PRODUKT{SPACES}OF         {  RET(MULTIPLICATION)  }
QUOSHUNT{SPACES}OF        {  RET(DIVISION)  }
MOD{SPACES}OF             {  RET(MODULO)  }
BIGGR{SPACES}OF           {  RET(BIGGER)  }
SMALLR{SPACES}OF          {  RET(SMALLER)  }

O{SPACES}RLY\?            {  RET(IF_BEGIN)  }
YA{SPACES}RLY             {  RET(IF_TRUE)  }
MEBBE                     {  RET(IF_ELSEIF)  }
NO{SPACES}WAI             {  RET(IF_FALSE)  }
OIC                       {  RET(IF_END)  }

IM{SPACES}IN              {  RET(CYCLE_BEGIN)  }
IM{SPACES}OUTTA           {  RET(CYCLE_END)  }

HOW{SPACES}DUZ{SPACES}I   {  RET(FUNCTION_BEGIN)  }
GTFO                      {  RET(FUNCTION_RETURN_NULL)  }
FOUND{SPACES}YR           {  RET(FUNCTION_RETURN)  }

IF{SPACES}U{SPACES}SAY{SPACES}SO {  RET(FUNCTION_END)  }

AN                        {  RET(ARG_SEPARATOR)  } 
A                         {  RET(CAST_SEPARATOR)  }
YR                        {  RET(SIGNATURE_SEPARATOR)  }


BOTH{SPACES}OF            {  RET(BOOL_AND)  }
EITHER{SPACES}OF          {  RET(BOOL_OR)   }
WON{SPACES}OF             {  RET(BOOL_XOR)  }
NOT                       {  RET(BOOL_NOT)  }
ALL{SPACES}OF             {  RET(BOOL_INF_AND)  }
ANY{SPACES}OF             {  RET(BOOL_INF_OR)  }
MKAY                      {  RET('\n')  }

BOTH{SPACES}SAEM          {  RET(EQUALS)  }
DIFFRINT                  {  RET(NOT_EQUALS)  }

VISIBLE                   {  RET(VISIBLE)  }
!                         {  RET(VISIBLE_FLAG) }
GIMMEH                    {  RET(GET_LINE)  }

SMOOSH                    {  RET(STR_CONCAT)  }

(WIN|FAIL)                {  
                              yylval.boolValue = (yytext[0] == 'W'); 
                              RET(BOOL_VALUE)  
                          }                     

BTW                       {  comment = COMMENT_SL; return SL_COMMENT;  }
OBTW                      {  comment = COMMENT_ML; return ML_COMMENT_BEGIN;  }
TLDR                      {  comment = COMMENT_DISABLED; return ML_COMMENT_END;  }
MAEK                      {  RET(CAST)  }
TROOF                     {  RET(BOOL_TYPE)  }
YARN                      {  RET(STR_TYPE)  }
NUMBR                     {  RET(INT_TYPE)  }
NUMBAR                    {  RET(FLOAT_TYPE)  }
NOOB                      {  RET(UNTYPED_TYPE)  }
IS{SPACES}NOW{SPACES}A    {  RET(VARIABLE_TYPE_CHANGE)  } 

{INTEGER}                 { 
                              sscanf(yytext, "%d", &yylval.intValue); 
                              RET(INT_NUMERAL) 
                          } 
({FLOAT}|{FLOAT_EXP})     { 
                              sscanf(yytext, "%f", &yylval.floatValue); 
                              RET(FLOAT_NUMERAL)
                          }
{STRING}                  {
                              strncpy(yylval.stringValue, yytext, 
                                      sizeof(yylval.stringValue));
                              RET(STRING_LITERAL)
                          }
{IDENT}                   { 
                                strncpy(yylval.varName, yytext,
                                      sizeof(yylval.varName));
                                RET(VARIABLE_ID);
                          }

,                         {  RET(',')  }

{SPACES}                  {  /* skip spaces */  }

\n                        {  
                              if (comment == COMMENT_SL) {
                                  comment = COMMENT_DISABLED;
                              }
                              if (lineContinuation) {
                                  lineContinuation = false;
                              } else {
                                  RET('\n');  
                              }
                          }

\.\.\.                    {
                              lineContinuation = true;                            
                          }

.                         { 
                              if (comment == COMMENT_DISABLED) {
                                  fprintf(stderr, "LexerError: unknown character '%c', line: %d\n", 
                                      yytext[0], yylineno);
                                      exit(-1);
                              }
                          }

%%

