%code requires {
  #include <memory>
  #include <string>
  #include "Ast/BaseAST.h"
  #include "Ast/CompUnitAST.h"
  #include "Ast/FuncDefAST.h"
  #include "Ast/FunctypeAST.h"
  #include "Ast/BlockAST.h"
  #include "Ast/StmtAST.h"
  #include "Ast/ExpAST.h"
  #include "Ast/UnaryExpAST.h"
  #include "Ast/PrimaryExpAST.h"
  #include "Ast/NumberAST.h"
}

%{
#include <iostream>
#include <memory>
#include <string>
#include "Ast/BaseAST.h"
#include "Ast/CompUnitAST.h"
#include "Ast/FuncDefAST.h"
#include "Ast/FunctypeAST.h"
#include "Ast/BlockAST.h"
#include "Ast/StmtAST.h"
#include "Ast/ExpAST.h"
#include "Ast/UnaryExpAST.h"
#include "Ast/PrimaryExpAST.h"
#include "Ast/NumberAST.h"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

%parse-param { std::unique_ptr<BaseAST> &ast }

%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

%token INT RETURN
%token <str_val> IDENT
%token <int_val> UNARY_OP
%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncType Block Stmt Exp UnaryExp PrimaryExp Number

%%

CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = std::move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

FuncType
  : INT {
    auto ast = new FunctypeAST();
    ast->type = string("int");
    $$ = ast;
  }
  ;

Block
  : '{' Stmt '}' {
    auto block = new BlockAST();
    block->stmt = unique_ptr<BaseAST>($2);
    $$ = block;
  }
  ;

Stmt
  : RETURN Exp ';' {
    auto stmt = new StmtAST();
    stmt->Exp = unique_ptr<BaseAST>($2);
    $$ = stmt;
  }
  ;
Exp
  : UnaryExp {
    auto exp = new ExpAST();
    exp->UnaryExp = unique_ptr<BaseAST>($1);
    $$ = exp;
  }
  ;
UnaryExp
  : PrimaryExp {
    auto unary = new UnaryExpAST();
    unary->PrimaryExp = unique_ptr<BaseAST>($1);
    $$ = unary;
  }
  | UNARY_OP UnaryExp {
    auto unary = new UnaryExpAST();
    unary->UnaryExp = unique_ptr<BaseAST>($2);
    unary->OpType = ( $1 == '+' ? PLUS : $1 == '-' ? MINUS : COMPLEMENT );
    $$ = unary;
  }
  ;
PrimaryExp
  : '(' Exp ')' {
    auto primary = new PrimaryExpAST();
    primary->Exp = unique_ptr<BaseAST>($2);
    $$ = primary;
  }
  | Number {
    auto primary = new PrimaryExpAST();
    primary->Number = unique_ptr<BaseAST>($1);
    $$ = primary;
  }
  ;

Number
  : INT_CONST {
    auto number = new NumberAST();
    number->value = $1;
    $$ = number;
  }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
