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

  #include "Ast/LValAST.h"
  #include "Ast/BlockItemAST.h"
  #include "Ast/DeclAST.h"
  #include "Ast/ConstDeclAST.h"
  #include "Ast/ConstExpAST.h"
  #include "Ast/ConstInitValAST.h"
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

#include "Ast/LValAST.h"
#include "Ast/BlockItemAST.h"
#include "Ast/DeclAST.h"
#include "Ast/ConstDeclAST.h"
#include "Ast/ConstExpAST.h"
#include "Ast/ConstInitValAST.h"

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
  std::vector<BaseAST*>* vec_val;
}

%token INT RETURN CONST
%token <str_val> IDENT
%token <int_val> UNARY_OP
%token <int_val> BINARY_OP
%token <int_val> LOGIC_OP_OR
%token <int_val> LOGIC_OP_AND
%token <int_val> COMP_OP_EQ
%token <int_val> COMP_OP_N

%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncType Block Stmt Exp UnaryExp PrimaryExp Number AddExp MulExp
%type <ast_val> LOrExp LAndExp EqExp RelExp

%type <ast_val> Decl ConstDecl BType ConstDef ConstInitVal BlockItem LVal ConstExp

%type <vec_val> BlockItems ConstDefs

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



Stmt
  : RETURN Exp ';' {
    auto stmt = new StmtAST();
    stmt->Exp = unique_ptr<BaseAST>($2);
    $$ = stmt;
  }
  ;
Exp
  : LOrExp {
    auto exp = new ExpAST();
    exp->LgExp = unique_ptr<BaseAST>($1);
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
LOrExp
  : LAndExp {
    auto lg_exp = new LgExpAST();
    lg_exp->LHS = unique_ptr<BaseAST>($1);
    $$ = lg_exp;
  }
  | LOrExp LOGIC_OP_OR LAndExp {
    auto lg_exp = new LgExpAST();
    lg_exp->LHS = unique_ptr<BaseAST>($1);
    lg_exp->type = static_cast<LogicOp>($2);
    lg_exp->RHS = unique_ptr<BaseAST>($3);
    $$ = lg_exp;
  }
  ;
LAndExp
  : EqExp {
    auto lg_exp = new LgExpAST();
    lg_exp->LHS = unique_ptr<BaseAST>($1);
    $$ = lg_exp;
  }
  | LAndExp LOGIC_OP_AND EqExp {
    auto lg_exp = new LgExpAST();
    lg_exp->LHS = unique_ptr<BaseAST>($1);
    lg_exp->type = static_cast<LogicOp>($2);
    lg_exp->RHS = unique_ptr<BaseAST>($3);
    $$ = lg_exp;
  }
  ;
EqExp
  : RelExp {
    auto eq_exp = new CompExpAST();
    eq_exp->LHS = unique_ptr<BaseAST>($1);
    $$ = eq_exp;
  }
  | EqExp COMP_OP_EQ RelExp {
    auto eq_exp = new CompExpAST();
    eq_exp->LHS = unique_ptr<BaseAST>($1);
    eq_exp->type = static_cast<CompOp>($2);
    eq_exp->RHS = unique_ptr<BaseAST>($3);
    $$ = eq_exp;
  }
  ;
RelExp
  : AddExp {
    auto rel_exp = new CompExpAST();
    rel_exp->LHS = unique_ptr<BaseAST>($1);
    $$ = rel_exp;
  }
  | RelExp COMP_OP_N AddExp {
    auto rel_exp = new CompExpAST();
    rel_exp->LHS = unique_ptr<BaseAST>($1);
    rel_exp->type = static_cast<CompOp>($2);
    rel_exp->RHS = unique_ptr<BaseAST>($3);
    $$ = rel_exp;
  }
  ;

Number
  : INT_CONST {
    auto number = new NumberAST();
    number->value = $1;
    $$ = number;
  }
  ;
Decl
  : ConstDecl {
    auto decl = new DeclAST();
    decl->ConstDecl = unique_ptr<BaseAST>($1);
    $$ = decl;
  }
  ;
ConstDecl
  : CONST BType ConstDef ConstDefs ';' {
    auto decl = new ConstDeclAST();
    decl->BType = unique_ptr<BaseAST>($2);
    decl->ConstDefs.push_back(unique_ptr<BaseAST>($3));
    if($4 == nullptr)
        $$ = decl;
    else{
        for(auto i:*($4)){
            decl->ConstDefs.push_back(unique_ptr<BaseAST>(i));
        }
        delete $4;
        $$ = decl;
    }
  }
  ;
ConstDefs
  : ConstDefs ',' ConstDef {
      if($1 == nullptr){
        auto vec = new vector<BaseAST*>;
        vec->push_back($3);
        $$ = vec;
      }
      else{
        $1->push_back($3);
        $$ = $1;
      }
  }
  | { $$ = nullptr; }
  ;
BType
  : INT {
    auto b = new BTypeAST();
    b->type = "int";
    $$ = b;
  }
  ;
ConstDef
  : IDENT '=' ConstInitVal {
    auto def = new ConstDefAST();
    def->ident = *unique_ptr<string>($1);
    def->ConstInitVal = unique_ptr<BaseAST>($3);
    $$ = def;
  }
  ;
ConstInitVal
  : ConstExp {
    auto val = new ConstInitValAST();
    val->ConstExp = unique_ptr<BaseAST>($1);
    $$ = val;
  }
  ;
ConstExp
  : Exp {
    auto exp = new ConstExpAST();
    exp->Exp = unique_ptr<BaseAST>($1);
    $$ = exp;
  }
  ;
LVal
  : IDENT {
    auto l_val = new LValAST();
    l_val->ident = *unique_ptr<string>($1);
    $$ = l_val;
  }
  ;
BlockItem
  : Decl {
    auto item = new BlockItemAST();
    item->Decl = unique_ptr<BaseAST>($1);
    $$ = item;
  }
  | Stmt {
    auto item = new BlockItemAST();
    item->Stmt = unique_ptr<BaseAST>($1);
    $$ = item;
  }
  ;
PrimaryExp
  : '(' Exp ')' {
    auto primary = new PrimaryExpAST();
    primary->Exp = unique_ptr<BaseAST>($2);
    $$ = primary;
  }
  | LVal {
    auto primary = new PrimaryExpAST();
    primary->LVal = unique_ptr<BaseAST>($1);
    $$ = primary;
  }
  | Number {
    auto primary = new PrimaryExpAST();
    primary->Number = unique_ptr<BaseAST>($1);
    $$ = primary;
  }
  ;
Block
  : '{' BlockItems '}' {
    auto block = new BlockAST();
    if($2 == nullptr)
        $$ = block;
    else{
        for(auto i: *$2){
            block->BlockItems.push_back(unique_ptr<BaseAST>(i));
        }
        delete $2;
        $$ = block;
    }
  }
  ;
BlockItems
  : BlockItems BlockItem {
      if($1 == nullptr){
        auto vec = new vector<BaseAST*>;
        vec->push_back($2);
        $$ = vec;
      }
      else{
        $1->push_back($2);
        $$ = $1;
      }
  }
  | { $$ = nullptr; }
  ;
%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
