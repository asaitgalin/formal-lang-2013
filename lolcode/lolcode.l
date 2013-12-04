%{
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "lolcode_stmt.h"

bool lineContinuation = false;

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
HAI                       {  return CODE_BEGIN;  }
KTHXBYE                   {  return CODE_END;  }
I{SPACES}HAS{SPACES}A     {  return VARIABLE_DECL;  }
ITZ                       {  return VARIABLE_INIT;  }
R                         {  return VARIABLE_ASSIGN;  }

IT                        {  return TEMP_VAR;  }

SUM{SPACES}OF             {  return ADDITION;  }
DIFF{SPACES}OF            {  return SUBTRACTION;  }
PRODUKT{SPACES}OF         {  return MULTIPLICATION;  }
QUOSHUNT{SPACES}OF        {  return DIVISION;  }
MOD{SPACES}OF             {  return MODULO;  }
BIGGR{SPACES}OF           {  return BIGGER;  }
SMALLR{SPACES}OF          {  return SMALLER;  }

AN                        {  return ARG_SEPARATOR;  } 
A                         {  return CAST_SEPARATOR;  }

BOTH{SPACES}OF            {  return BOOL_AND;  }
EITHER{SPACES}OF          {  return BOOL_OR;   }
WON{SPACES}OF             {  return BOOL_XOR;  }
NOT                       {  return BOOL_NOT;  }
ALL{SPACES}OF             {  return BOOL_INF_AND;  }
ANY{SPACES}OF             {  return BOOL_INF_OR;  }
MKAY                      {  return '\n';  }

BOTH{SPACES}SAEM          {  return EQUALS;  }
DIFFRINT                  {  return NOT_EQUALS;  }

VISIBLE                   {  return VISIBLE;  }
!                         {  return VISIBLE_FLAG; }
GIMMEH                    {  return GET_LINE;  }

SMOOSH                    {  return STR_CONCAT;  }

(WIN|FAIL)                {  
                              yylval.boolValue = (yytext[0] == 'W'); 
                              return BOOL_VALUE;  
                          }                     

MAEK                      {  return CAST;  }
TROOF                     {  return BOOL_TYPE;  }
YARN                      {  return STR_TYPE;  }
NUMBR                     {  return INT_TYPE;  }
NUMBAR                    {  return FLOAT_TYPE;  }
NOOB                      {  return UNTYPED_TYPE;  }
IS{SPACES}NOW{SPACES}A    {  return VARIABLE_TYPE_CHANGE;  } 

{INTEGER}                 { 
                              sscanf(yytext, "%d", &yylval.intValue); 
                              return INT_NUMERAL; 
                          } 
({FLOAT}|{FLOAT_EXP})     { 
                              sscanf(yytext, "%f", &yylval.floatValue); 
                              return FLOAT_NUMERAL; 
                          }
{STRING}                  {
                              strncpy(yylval.stringValue, yytext, 
                                      sizeof(yylval.stringValue));
                              return STRING_LITERAL;
                          }
{IDENT}                   { 
                              strncpy(yylval.varName, yytext,
                                      sizeof(yylval.varName));
                              return VARIABLE_ID; 
                          }

,                         {  return ',';  }

{SPACES}                  {  /* skip spaces */  }

\n                        {  
                              if (lineContinuation) {
                                  lineContinuation = false;
                              } else {
                                  return '\n';  
                              }
                          }

\.\.\.                    {
                              lineContinuation = true;                            
                          }

.                         { 
                              fprintf(stderr, "LexerError: unknown character '%c', line: %d\n", 
                                      yytext[0], yylineno);
                              exit(-1);
                          }
%%

