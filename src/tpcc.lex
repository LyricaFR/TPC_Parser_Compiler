%{
#include <stdio.h>
#include "src/tree.h"
#include "tpcc.tab.h"  
int lineno = 1;
int tok_end = 1;
%}

%option nounput
%option noinput
%option noyywrap
%x COMMENT1 COMMENT2 CHAR STRING

letter [a-zA-Z]
num [0-9]
%%

'                                    {BEGIN CHAR;}
<CHAR>[\\]n                          {tok_end = 1; return CHARACTER;}
<CHAR>'                              {BEGIN INITIAL;}
<CHAR>.                              {tok_end += yyleng; yylval.byte = yytext[0]; return CHARACTER;}
{num}+                               {tok_end += yyleng; yylval.num = atoi(yytext); return NUM;}
if                                   {tok_end += yyleng; return IF;}
else                                 {tok_end += yyleng; return ELSE;}
return                               {tok_end += yyleng; return RETURN;}
for                                  {tok_end += yyleng; return FOR;}
switch                               {tok_end += yyleng; return SWITCH;}
case                                 {tok_end += yyleng; return CASE;}
default                              {tok_end += yyleng; return DEFAULT;}
break                                {tok_end += yyleng; return BREAK;}
while                                {tok_end += yyleng; return WHILE;}
int|char                             {tok_end += yyleng; strcpy(yylval.type, yytext); return TYPE;}
void                                 {tok_end += yyleng; return VOID;}
[*/%]                                {tok_end += yyleng; yylval.byte = yytext[0]; return DIVSTAR;}
[+-]                                 {tok_end += yyleng; yylval.byte = yytext[0]; return ADDSUB;}
[=!;,(){}:]                          {tok_end += yyleng; return yytext[0];}
"/*"                                 {BEGIN COMMENT1;}
<COMMENT1>"*/"                       {BEGIN INITIAL;}
"//"                                 {BEGIN COMMENT2;}
<COMMENT2>\n                         {BEGIN INITIAL; lineno++;}
<COMMENT1>\n                         {tok_end = 1; lineno++;}
<COMMENT1,COMMENT2>.                 ;
"=="|"!="                            {tok_end += yyleng; strcpy(yylval.comp, yytext); return EQ;}
"&&"                                 {tok_end += yyleng; return AND;}
"||"                                 {tok_end += yyleng; return OR;}
({letter}|_)({letter}|[0-9]|_)*      {tok_end += yyleng; strcpy(yylval.ident, yytext); return IDENT;}
"<"|"<="|">"|">="                    {tok_end += yyleng; strcpy(yylval.comp, yytext); return ORDER;}
\n                                   {tok_end = 1; lineno++;}
.                                    {tok_end += yyleng;}
<<EOF>>                              {return 0;};

%%