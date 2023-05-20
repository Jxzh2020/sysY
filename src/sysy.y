%code requires {
  #include <memory>
  #include <string>
  #include "Ast/BaseAST.h"
  #include "Ast/CompUnitAST.h"
  #include "Ast/FuncDefAST.h"
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

  #include "Ast/VarDeclAST.h"
  #include "Ast/VarDefAST.h"
  #include "Ast/InitValAST.h"

  #include "Ast/FuncFParamAST.h"
  #include "Ast/PrimitiveTypeAST.h"
}

%{
#include <iostream>
#include <memory>
#include <string>
#include "Ast/BaseAST.h"
#include "Ast/CompUnitAST.h"
#include "Ast/FuncDefAST.h"
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

#include "Ast/VarDeclAST.h"
#include "Ast/VarDefAST.h"
#include "Ast/InitValAST.h"

#include "Ast/FuncFParamAST.h"
#include "Ast/PrimitiveTypeAST.h"

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

%token INT RETURN CONST VOID IF ELSE WHILE CONTINUE BREAK
%token <str_val> IDENT
%token <int_val> UNARY_OP
%token <int_val> BINARY_OP
%token <int_val> LOGIC_OP_OR
%token <int_val> LOGIC_OP_AND
%token <int_val> COMP_OP_EQ
%token <int_val> COMP_OP_N

%token <int_val> INT_CONST

%type <ast_val> FuncDef PrimitiveType Block Stmt Exp UnaryExp PrimaryExp Number AddExp MulExp
%type <ast_val> LOrExp LAndExp EqExp RelExp

%type <ast_val> Decl ConstDecl ConstDef ConstInitVal BlockItem LVal ConstExp
%type <ast_val> VarDecl InitVal VarDef

%type <ast_val> FuncFParam

%type <vec_val> BlockItems ConstDefs VarDefs ConstExps
%type <vec_val> FuncFParams DFuncFParams FuncRParams DExps CompUnits
%type <ast_val> Matched Unmatched
%%

CompUnit
  : CompUnits {
    auto comp_unit = make_unique<CompUnitAST>();
    if($1 == nullptr){
        cout << "Empty File?" << endl;
        exit(0);
    }
    for(auto i: *$1)
        comp_unit->globe.push_back(unique_ptr<BaseAST>(i));
    delete $1;
    ast = std::move(comp_unit);
  }
  ;
CompUnits
  : CompUnits FuncDef {
    $1->push_back($2);
    $$ = $1;
  }
  | CompUnits Decl {
    $1->push_back($2);
    $$ = $1;
  }
  | Decl {
      auto units = new vector<BaseAST*>;
      units->push_back($1);
      $$ = units;
  }
  | FuncDef {
    auto units = new vector<BaseAST*>;
    units->push_back($1);
    $$ = units;
  }
  ;
FuncDef
  : PrimitiveType IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    if($4 != nullptr){
        for(auto i : *$4)
            ast->params.push_back(unique_ptr<BaseAST>(i));
        delete $4;
        ast->block = unique_ptr<BaseAST>($6);
        $$ = ast;
    }
    else{
        ast->block = unique_ptr<BaseAST>($6);
        $$ = ast;
    }
  }
  ;
FuncFParams
  : DFuncFParams {
    $$ = $1;
  }
  | { $$ = nullptr; }
DFuncFParams
  : DFuncFParams ',' FuncFParam {
    $1->push_back($3);
    $$ = $1;
  }
  | FuncFParam {
    auto vec = new vector<BaseAST*>;
    vec->push_back($1);
    $$ = vec;
  }
  ;
FuncFParam
  : PrimitiveType IDENT {
    auto para = new FuncFParamAST();
    para->PrimitiveType = unique_ptr<BaseAST>($1);
    para->ident = *unique_ptr<string>($2);
    $$ = para;
  };
PrimitiveType
  : INT {
    auto ast = new PrimitiveTypeAST();
    ast->type = string("int");
    $$ = ast;
  }
  | VOID {
    auto ast = new PrimitiveTypeAST();
    ast->type = string("void");
    $$ = ast;
  }
  ;
Stmt
  : Matched {
    dynamic_cast<StmtAST*>($1)->matched = true;
    $$ = $1;
  }
  | Unmatched {
    dynamic_cast<StmtAST*>($1)->matched = false;
    $$ = $1;
  }
  ;
Matched
  : RETURN ';' {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::RET;
    $$ = stmt;
  }
  | RETURN Exp ';' {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::RET;
    stmt->Exp = unique_ptr<BaseAST>($2);
    $$ = stmt;
  }
  | LVal '=' Exp ';' {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::ASSIGN;
    stmt->LVal = unique_ptr<BaseAST>($1);
    stmt->Exp = unique_ptr<BaseAST>($3);
    $$ = stmt;
  }
  | Block {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::BLOCK;
    stmt->Block = unique_ptr<BaseAST>($1);
    $$ = stmt;
  }
  | Exp ';' {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::EXP;
    stmt->Exp = unique_ptr<BaseAST>($1);
    $$ = stmt;
  }
  | ';' {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::EXP;
    $$ = stmt;
  }
  | IF '(' Exp ')' Matched ELSE Matched {
    dynamic_cast<StmtAST*>($5)->matched = true;
    dynamic_cast<StmtAST*>($7)->matched = true;

    auto stmt = new StmtAST();
    stmt->type = StmtAST::IF;
    stmt->Exp = unique_ptr<BaseAST>($3);
    stmt->if_stmt = unique_ptr<BaseAST>($5);
    stmt->else_stmt = unique_ptr<BaseAST>($7);
    $$ = stmt;
  }
  | BREAK ';' {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::BREAK;
    $$ = stmt;
  }
  | CONTINUE ';' {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::CONTINUE;
    $$ = stmt;
  }
  | WHILE '(' Exp ')' Stmt {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::WHILE;
    stmt->Exp = unique_ptr<BaseAST>($3);
    stmt->if_stmt = unique_ptr<BaseAST>($5);
    $$ = stmt;
  }
  ;
Unmatched
  : IF '(' Exp ')' Stmt {
    auto stmt = new StmtAST();
    stmt->type = StmtAST::IF;
    stmt->Exp = unique_ptr<BaseAST>($3);
    stmt->if_stmt = unique_ptr<BaseAST>($5);
    $$ = stmt;
  }
  | IF '(' Exp ')' Matched ELSE Unmatched {
    dynamic_cast<StmtAST*>($5)->matched = true;
    dynamic_cast<StmtAST*>($7)->matched = false;

    auto stmt = new StmtAST();
    stmt->type = StmtAST::IF;
    stmt->Exp = unique_ptr<BaseAST>($3);
    stmt->if_stmt = unique_ptr<BaseAST>($5);
    stmt->else_stmt = unique_ptr<BaseAST>($7);
    $$ = stmt;
  };
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
  | IDENT '(' FuncRParams ')' {
    auto unary = new UnaryExpAST();
    unary->ident = *unique_ptr<string>($1);
    if($3 == nullptr)
        $$ = unary;
    else{
        for(auto i: *$3)
            unary->params.push_back(unique_ptr<BaseAST>(i));
        delete $3;
        $$ = unary;
    }
  }
  ;
FuncRParams
  : Exp {
    auto param = new vector<BaseAST*>;
    param->push_back($1);
    $$ = param;
  }
  | DExps ',' Exp { // keeping the parameter order is important
    $1->push_back($3);
    $$ = $1;
  }
  | { $$ = nullptr; }
  ;
DExps
  : DExps ',' Exp {
    $1->push_back($3);
    $$ = $1;
  }
  | Exp {
    auto param = new vector<BaseAST*>;
    param->push_back($1);
    $$ = param;
  };

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
  | VarDecl {
    auto decl = new DeclAST();
    decl->VarDecl = unique_ptr<BaseAST>($1);
    $$ = decl;
  }
  ;
ConstDecl
  : CONST PrimitiveType ConstDef ConstDefs ';' {
    auto decl = new ConstDeclAST();
    decl->PrimitiveType = unique_ptr<BaseAST>($2);
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
ConstDef
  : IDENT '=' ConstInitVal {
    auto def = new ConstDefAST();
    def->ident = *unique_ptr<string>($1);
    def->ConstInitVal = unique_ptr<BaseAST>($3);
    $$ = def;
  }
  | IDENT '[' ConstExp ']' '=' ConstInitVal {
    auto def = new ConstDefAST();
    def->ident = *unique_ptr<string>($1);
    def->ConstExp = unique_ptr<BaseAST>($3);
    def->ConstInitVal = unique_ptr<BaseAST>($6);
    $$ = def;
  }
  ;
ConstInitVal
  : ConstExp {
    auto val = new ConstInitValAST();
    val->ConstExp = unique_ptr<BaseAST>($1);
    $$ = val;
  }
  | '{' ConstExps '}' {
    auto val = new ConstInitValAST();
    for(auto i: *$2){
        val->vals.push_back(unique_ptr<BaseAST>(i));
    }
    delete $2;
    $$ = val;
  }
  ;
ConstExps
 : ConstExps ',' ConstExp {
    $1->push_back($3);
    $$ = $1;
 }
 | ConstExp {
    auto vec = new vector<BaseAST*>;
    vec->push_back($1);
    $$ = vec;
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
  | IDENT '[' Exp ']' {
    auto l_val = new LValAST();
    l_val->ident = *unique_ptr<string>($1);
    l_val->Exp = unique_ptr<BaseAST>($3);
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
    // 可能是空块
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
InitVal
  : Exp {
    auto val = new InitValAST();
    val->Exp = unique_ptr<BaseAST>($1);
    $$ = val;
  }
  //
  | '{' DExps '}' {
    auto val = new ExpAST();
    for(auto i: *$2){
        val->vals.push_back(unique_ptr<BaseAST>(i));
    }
    delete $2;
    $$ = val;
  }
  ;
VarDecl
  : PrimitiveType VarDef VarDefs ';' {
    auto var = new VarDeclAST();
    var->PrimitiveType = unique_ptr<BaseAST>($1);
    var->VarDefs.push_back(unique_ptr<BaseAST>($2));
    if($3 == nullptr)
        $$ = var;
    else{
        for(auto i:*($3)){
            var->VarDefs.push_back(unique_ptr<BaseAST>(i));
        }
        delete $3;
        $$ = var;
    }
  }
  ;
VarDefs
  : VarDefs ',' VarDef {
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
VarDef
  : IDENT '=' InitVal {
    auto var = new VarDefAST();
    var->ident = *unique_ptr<string>($1);
    var->InitVal = unique_ptr<BaseAST>($3);
    $$ = var;
  }
  | IDENT '[' ConstExp ']' '=' InitVal {
    auto def = new VarDefAST();
    def->ident = *unique_ptr<string>($1);
    def->ConstExp = unique_ptr<BaseAST>($3);
    def->InitVal = unique_ptr<BaseAST>($6);
    $$ = def;
  }
  | IDENT {
    auto var = new VarDefAST();
    var->ident = *unique_ptr<string>($1);
    $$ = var;
  }
  | IDENT '[' ConstExp ']'{
    auto var = new VarDefAST();
    var->ident = *unique_ptr<string>($1);
    var->ConstExp = unique_ptr<BaseAST>($3);
    $$ = var;
  }
  ;
%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
