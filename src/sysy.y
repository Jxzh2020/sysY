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
  #include "Ast/MulExpAST.h"
  #include "Ast/AddExpAST.h"

  #include "Ast/LgExpAST.h"
  #include "Ast/CompExpAST.h"
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
#include "Ast/MulExpAST.h"
#include "Ast/AddExpAST.h"

#include "Ast/LgExpAST.h"
#include "Ast/CompExpAST.h"

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
%token <int_val> BINARY_OP
%token <str_val> LOGIC_OP
%token <str_val> COMP_OP

%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncType Block Stmt Exp UnaryExp PrimaryExp Number AddExp MulExp
%type <ast_val> LOrExp LAndExp EqExp RelExp

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
  : AddExp {
    auto exp = new ExpAST();
    exp->AddExp = unique_ptr<BaseAST>($1);
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
MulExp
  : UnaryExp {
    auto mul_exp = new MulExpAST();
    mul_exp->UnaryExp = unique_ptr<BaseAST>($1);
    $$ = mul_exp;
  }
  | MulExp BINARY_OP UnaryExp {
    auto mul_exp = new MulExpAST();
    mul_exp->MulExp = unique_ptr<BaseAST>($1);
    mul_exp->type = $2 == '*' ? MUL : $2 == '/' ? DIV : MOD;
    mul_exp->UnaryExp = unique_ptr<BaseAST>($3);
    $$ = mul_exp;
  }
  ;
AddExp
  : MulExp {
    auto add_exp = new AddExpAST();
    add_exp->MulExp = unique_ptr<BaseAST>($1);
    $$ = add_exp;
  }
  | AddExp UNARY_OP MulExp {
    auto add_exp = new AddExpAST();
    add_exp->AddExp = unique_ptr<BaseAST>($1);
    add_exp->type = $2 == '+' ? PLUS : $2 == '-' ? MINUS : COMPLEMENT ;
    add_exp->MulExp = unique_ptr<BaseAST>($3);
    $$ = add_exp;
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
