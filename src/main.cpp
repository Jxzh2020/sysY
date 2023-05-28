/*
 *
    CompUnit    ::= FuncDef FuncDefs ;
    FuncDef     ::= FuncType IDENT "(" [FuncFParams] ")" Block;
    FuncType    ::= "void" | "int";

    UnaryExp    ::= PrimaryExp | UnaryOp UnaryExp | IDENT "(" [FuncRParams] ")";

    Number      ::= INT_CONST;
    UnaryOp     ::= "+" | "-" | "!";
    MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
    AddExp      ::= MulExp | AddExp ("+" | "-") MulExp;

    Exp         ::= LOrExp;
    RelExp      ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
    EqExp       ::= RelExp | EqExp ("==" | "!=") RelExp;
    LAndExp     ::= EqExp | LAndExp "&&" EqExp;
    LOrExp      ::= LAndExp | LOrExp "||" LAndExp;

 Trying to eliminate some AST derived class:
 Logic Expression: LgExpAST containing LOrExp, LAndExp, EqExp
 Logic Operation:  OR ||, AND &&,

 Comparison Expression: CompExpAst containing RelExp
 Comparison Operation: LT <, GT >, LE <=, GE >=, EQ ==, NEQ !=

// adding new rules...

    BType         ::= "int";
    ConstDef      ::= IDENT "=" ConstInitVal;
    ConstInitVal  ::= ConstExp;
    ConstExp      ::= Exp;
    LVal          ::= IDENT;
    BlockItem     ::= Decl | Stmt;
->  PrimaryExp    ::= "(" Exp ")" | LVal | Number;
 // here the first time LVal is selected, TODO here all are read only!

 // here modified a little bit

    ConstDecl     ::= "const" BType ConstDef ConstDefs ";";
    ConstDefs     ::= ConstDefs "," ConstDef | ;
    Block         ::= "{" BlockItems "}";
    BlockItems    ::= BlockItems BlockItem | ;

***********
    Decl          ::= ConstDecl | VarDecl;
    VarDef        ::= IDENT | IDENT "=" InitVal;
    VarDecl       ::= BType VarDef {"," VarDef} ";";
    InitVal       ::= Exp;

****************语句块与作用域
 *
 * 跳过 if 语句，while 语句
 *
****************函数
    FuncDefs    ::= FuncDefs FuncDef | ;
    FuncFParam  ::= BType IDENT;
    FuncFParams ::= FuncFParam | DFuncFParams ',' FuncFParam | ;
    DFuncFParams ::= DFuncFParams ',' FuncFParam | FuncFParam ;
    FuncRParams ::= Exp {"," Exp};

****************
 *
 * 跳过sysY库函数
 *
*****************
    CompUnit ::= [CompUnit] (Decl | FuncDef);
*****************
 *
 * if 语句
 *

Stmt          ::= LVal "=" Exp ";"
                | [Exp] ";"
                | Block
                | "if" "(" Exp ")" Stmt ["else" Stmt]
                | "return" [Exp] ";";


---> Not good for parsing and constructing AST :    "if" "(" Exp ")" Stmt ["else" Stmt]
     Now change it to the follow derivation:

     // note this contains priority of if-else combination implicitly!
     Stmt -> IF '(' Exp ')' Stmt ElseClause
     ElseClause -> ELSE Stmt | ;
     // above would cause reduce shift conflict in bison,
     // now, we define that Stmt between if and else must not be unpaired if-else,
     // namely any if-else between another if-else must be paired.

     Stmt -> Matched | Unmatched
     Matched -> IF '(' Exp ')' Matched Else Matched | Origin_Stmt
     Unmatched -> IF '(' Exp ')' Stmt | IF '(' Exp ')' Matched ELSE Unmatched

 *
*****************
 * Stmt       ::= "while" "(" Exp ")" Stmt
                | "break" ";"
                | "continue" ";"
                | ...
 *
*****************
 *
 *
 * To fix continue or break,
 * both the ast node would set IR::hasBranch true.
 * there are three derivations:
 *
 *
 *      {
 *          ...
 *          ...
 *          ...
 *          break;
 *          ... // delete
 *          ... // delete
 *      }
 *
 *      Block <- Stmt   : end the invalid insts parsing.        ---> upper level cancel logical end
 *      IF <- Stmt      : replace the original flow control     ---> no upper level influence ?
 *      WHILE <- Stmt   : replace the original flow control     ---> no upper level influence ?
 *
 *
 *  Note:
 *      Only if, else, while these three kind of stmt can ClearBranch,
 *      others don't have flow control
 *
 *  if and else all branch
 *
 * */


#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include "Ast/BaseAST.h"

#define GEP_TEST
using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段

extern FILE *yyin;

extern int yyparse(unique_ptr<BaseAST> &ast);

void gep_test();

int main(int argc, const char *argv[]) {
#ifdef GEP_TEST
    gep_test();
    return 0;
#endif

    // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
    // compiler 模式 输入文件 -o 输出文件
    //assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    auto output = argv[4];

    // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
    //yyin = fopen(input, "r");
    yyin = fopen("demo.c", "r");
    assert(yyin);
    std::cout << "lexer pass" << std::endl;
    // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
    unique_ptr<BaseAST> ast;

    auto ret = yyparse(ast);
    assert(!ret);
    std::cout << "passer pass" << std::endl;

    ast->codegen();

    std::ofstream file("demo.ll");
    file << IR::get()->getModule()->print().str();

    file.close();

    return 0;
}

void gep_test(){
    auto builder = IR::get()->getBuilder().get();
    auto F = IRGen::Function::Create(IRGen::FunctionType::get(IRGen::Type::getInt32()),IRGen::Function::ExternalLinkage,"main",IR::get()->getModule().get());
    IR::get()->EnterFunc(F);
    F = IRGen::Function::Create(IRGen::FunctionType::get(IRGen::Type::getInt32(),{ IRGen::Type::getPtr()}),IRGen::Function::ExternalLinkage,"test",IR::get()->getModule().get());

    IR::get()->NewLogicalBlockStart();
    auto inst = builder->CreateAlloca(IRGen::Type::getArray(IRGen::Type::getInt32(),5),"array");
    IR::get()->AddAlloca(inst,"array");

    inst = builder->CreateGEP(IRGen::Type::getArray(IRGen::Type::getInt32(),5),IR::get()->GetAlloca("array"),{IRGen::Constant::get(IRGen::Type::getInt32(),0),IRGen::Constant::get(IRGen::Type::getInt32(),0)});
    builder->CreateStore(IRGen::Constant::get(IRGen::Type::getInt32(),2233),inst);
    builder->CreateCall( IR::get()->getModule()->get_function("test"), {inst});
    builder->CreateStore(IRGen::Constant::get(IRGen::Type::getInt32(),3322),inst);
    inst = builder->CreateLoad(inst);
    builder->CreateCall( IR::get()->getModule()->get_function("putint"), {inst});

    builder->CreateRet(IRGen::Constant::get(IRGen::Type::getInt32(),1));
    IR::get()->IR::NewLogicalBlockEnd();

    auto argIter = F->arg_begin();
    argIter->get()->set_name("array");
    IR::get()->EnterFunc(F);
    IR::get()->NewLogicalBlockStart();
    {
        auto arg = builder->CreateAlloca(argIter->get()->get_type(),argIter->get()->get_name(), false);
        IR::get()->AddAlloca(arg,argIter->get()->get_name());
        builder->CreateStore(argIter->get(),arg);
    }
    {

        auto temp = IR::get()->GetAlloca("array");
        inst = builder->CreateGEP(IRGen::Type::getArray(IRGen::Type::getInt32(),5),builder->CreateLoad(temp),{IRGen::Constant::get(IRGen::Type::getInt32(),0),IRGen::Constant::get(IRGen::Type::getInt32(),0)});

        inst = builder->CreateLoad(inst);
        builder->CreateCall( IR::get()->getModule()->get_function("putint"), {inst});

        builder->CreateRet(IRGen::Constant::get(IRGen::Type::getInt32(),1));

        IR::get()->IR::NewLogicalBlockEnd();
    }


    std::ofstream file("demo.ll");
    file << IR::get()->getModule()->print().str();

    file.close();

}
