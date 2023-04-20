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

static int com_depth;
static bool is_comment(){ if(com_depth == 0) return false; return true; }

%}

/* 空白符和注释 */
WhiteSpace    [ \t\n\r]*
LineComment   "//".*
BlockCommentS "/*"
BlockCommentE "*/"

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

%%

{WhiteSpace}    { /* 忽略, 不做任何操作 */ }
{LineComment}   { /* 忽略, 不做任何操作 */ }

{BlockCommentS} { com_depth++; }
{BlockCommentE} { com_depth--; }

"int"           { if(!is_comment()) return INT; }
"return"        { if(!is_comment()) return RETURN; }

{Identifier}    { if(!is_comment()) { yylval.str_val = new string(yytext); return IDENT; } }

{Decimal}       { if(!is_comment()) { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; } }
{Octal}         { if(!is_comment()) { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; } }
{Hexadecimal}   { if(!is_comment()) { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; } }

{UnaryOp} { if(!is_comment()) { yylval.int_val = (int)yytext[0]; return UNARY_OP; } }

{BinaryOp} { if(!is_comment()) { yylval.int_val = (int)yytext[0]; return BINARY_OP; } }

.               { if(!is_comment()) return yytext[0]; }

%%
