%option noyywrap
%option nounput
%option noinput

%{

#include <cstdlib>
#include <string>

// 因为 Flex 会用到 Bison 中关于 token 的定义
// 所以需要 include Bison 生成的头文件
#include "sysy.tab.hpp"

using namespace std;

%}

/* 空白符和注释 */
WhiteSpace    [ \t\n\r]*
LineComment   "//".*

BlockComment  "/*"([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+"/"



/* 标识符 */
Identifier    [a-zA-Z_][a-zA-Z0-9_]*

/* 整数字面量 */
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

/* 一元操作符 */
UnaryOp [-+!]

/* 二元操作符 */
BinaryOp [*/%]

/* 逻辑操作符 */
LogicOp_OR \|\|

LogicOp_AND &&

/* 比较操作符 */
CompOp_EQ [=!]=

CompOp_N [><]=?

%%

{WhiteSpace}    { /* 忽略, 不做任何操作 */ }
{LineComment}   { /* 忽略, 不做任何操作 */ }

"int"           { return INT; }
"return"        { return RETURN; }
"const"         { return CONST; }

{Identifier}    { yylval.str_val = new string(yytext); return IDENT; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }

{UnaryOp} { yylval.int_val = (int)yytext[0]; return UNARY_OP; }

{BinaryOp} { yylval.int_val = (int)yytext[0]; return BINARY_OP; }

{LogicOp_OR} {
    yylval.int_val = OR;
    return LOGIC_OP_OR;
}
{LogicOp_AND} {
    yylval.int_val = AND;
    return LOGIC_OP_AND;
}
{CompOp_EQ} {
    if(yytext[0] == '=')
        yylval.int_val = EQ;
    else
        yylval.int_val = NEQ;
    return COMP_OP_EQ;
}

{CompOp_N} {
    if(yytext[0] == '>'){
        if(yytext[1] == '\n')
            yylval.int_val = GT;
        else
            yylval.int_val = GE;
    }
    else{
        if(yytext[1] == '\n')
            yylval.int_val = LT;
        else
            yylval.int_val = LE;
    }
    return COMP_OP_N;
}

{BlockComment} { /* 忽略块注释的内容 */ }

.               {  return yytext[0]; }

%%
