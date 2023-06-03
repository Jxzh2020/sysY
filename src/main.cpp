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


---> Not good for parsing and constructing AST :    "if" "(" Exp ")" Stmt
["else" Stmt] Now change it to the follow derivation:

     // note this contains priority of if-else combination implicitly!
     Stmt -> IF '(' Exp ')' Stmt ElseClause
     ElseClause -> ELSE Stmt | ;
     // above would cause reduce shift conflict in bison,
     // now, we define that Stmt between if and else must not be unpaired
if-else,
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
 *      Block <- Stmt   : end the invalid insts parsing.        ---> upper level
cancel logical end
 *      IF <- Stmt      : replace the original flow control     ---> no upper
level influence ?
 *      WHILE <- Stmt   : replace the original flow control     ---> no upper
level influence ?
 *
 *
 *  Note:
 *      Only if, else, while these three kind of stmt can ClearBranch,
 *      others don't have flow control
 *
 *  if and else all branch
 *
 * */

#include "Ast/BaseAST.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>

// #define GEP_TEST
using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段

extern FILE *yyin;

extern int yyparse(unique_ptr<BaseAST> &ast);

void gep_test();

// void printhelp() {
//   cout << "How to use MySysY compiler:" << endl;
//   cout << "-help: print this message" << endl;
//   cout << "-i: A MUST ARGUMENT, source code .c file." << endl;
//   cout << "-o: generate output file, which is a .o file. If you ignore
//   this "
//           "argument, the output file will be named a.o"
//        << endl;
//   cout << "-l: generate llvm ir code file, which is a .ll file. If you
//   ignore "
//           "this argument, llvm ir will be print in the console"
//        << endl;
//   cout << "-v: generate a HTML visualization file, which is a .html file,
//   in "
//           "which the syntax tree structure are shown. If you ignore this
//           " "argument, the HTML file will not generate"
//        << endl;
//   cout << "-O: the level of optimization, supported levels: -O0, -O1,
//   -O2, "
//           "-O3, -Oz, -Os. If you ignore this argument, it will not
//           optimize"
//        << endl;
//   cout << "For a instance: " << endl;
//   cout << "If you want to compile source file test.c, get the output file
//   "
//           "out.o, get the llvm ir code file ir.ll, get the visualization
//           file " "visual.html and optimize program in level O2"
//        << endl;
//   cout << "You should use command line: MySysY -i test.c -o out.o -l -v
//   -O2"
//        << endl;
//   return;
// }

// void GenerateVisualization(unique_ptr<BaseAST> &ast, string
// VisualizationFile) {
//   // visualization test
//   extern const char *Html;
//   std::string OutputString = Html;
//   std::string Json = ast->astJson(1000);
//   // std::cout << Json;
//   std::string Target = "${ASTJson}";
//   auto Pos = OutputString.find(Target);
//   OutputString.replace(Pos, Target.length(), Json.c_str());
//   std::ofstream HTMLFile(VisualizationFile);
//   HTMLFile << OutputString;
//   HTMLFile.close();
// }

// void GenerateObject(string OutputFile) {
//   auto TargetTriple = llvm::sys::getDefaultTargetTriple();
//   llvm::InitializeAllTargetInfos();
//   llvm::InitializeAllTargets();
//   llvm::InitializeAllTargetMCs();
//   llvm::InitializeAllAsmParsers();
//   llvm::InitializeAllAsmPrinters();
//   std::string Error;
//   auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
//   if (!Target) {
//     throw runtime_error(Error);
//     return;
//   }
//   auto CPU = "generic";
//   auto Features = "";
//   llvm::TargetOptions opt;
//   auto RM = llvm::Optional<llvm::Reloc::Model>();
//   auto TargetMachine =
//       Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);
//   IR::get()->getModule()->setDataLayout(TargetMachine->createDataLayout());
//   IR::get()->getModule()->setTargetTriple(TargetTriple);
//   std::error_code EC;
//   llvm::raw_fd_ostream Dest(OutputFile, EC, llvm::sys::fs::OF_None);
//   if (EC) {
//     throw std::runtime_error("Could not open file: " + EC.message());
//     return;
//   }
//   auto FileType = llvm::CGFT_ObjectFile;
//   llvm::legacy::PassManager PM;
//   if (TargetMachine->addPassesToEmitFile(PM, Dest, nullptr, FileType)) {
//     throw std::runtime_error("TargetMachine can't emit a file of this
//     type"); return;
//   }
//   PM.run(*IR::get()->getModule());
//   Dest.flush();
// }

// void IROptimize(string OptimizeLevel) {
//   llvm::LoopAnalysisManager LAM;
//   llvm::FunctionAnalysisManager FAM;
//   llvm::CGSCCAnalysisManager CGAM;
//   llvm::ModuleAnalysisManager MAM;
//   // Create the new pass manager builder.
//   llvm::PassBuilder PB;
//   // Register all the basic analyses with the managers.
//   PB.registerModuleAnalyses(MAM);
//   PB.registerCGSCCAnalyses(CGAM);
//   PB.registerFunctionAnalyses(FAM);
//   PB.registerLoopAnalyses(LAM);
//   PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
//   const llvm::OptimizationLevel *OptLevel;
//   if (OptimizeLevel == "") {
//     OptLevel = &llvm::OptimizationLevel::O0;
//   }
//   if (OptimizeLevel != "") {
//     if (OptimizeLevel == "O0")
//       OptLevel = &llvm::OptimizationLevel::O0;
//     else if (OptimizeLevel == "O1")
//       OptLevel = &llvm::OptimizationLevel::O1;
//     else if (OptimizeLevel == "O2")
//       OptLevel = &llvm::OptimizationLevel::O2;
//     else if (OptimizeLevel == "O3")
//       OptLevel = &llvm::OptimizationLevel::O3;
//     else if (OptimizeLevel == "Os")
//       OptLevel = &llvm::OptimizationLevel::Os;
//     else if (OptimizeLevel == "Oz")
//       OptLevel = &llvm::OptimizationLevel::Oz;
//   }
//   llvm::ModulePassManager MPM =
//   PB.buildPerModuleDefaultPipeline(*OptLevel);
//   // Optimize the IR
//   MPM.run(*IR::get()->getModule(), MAM);
//   return;
// }

int main(int argc, const char *argv[]) {
#ifdef GEP_TEST
    gep_test();
    return 0;
#endif

    // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
    // compiler 模式 输入文件 -o 输出文件
    // assert(argc == 5);
    // bool help = false;
    // for(int i = 1; i < argc; i++)
    // {
    //     string s(argv[i]);
    //     if(s=="-help")
    //     {
    //         help = true;
    //         break;
    //     }
    // }
    // if (help || argc == 1)
    // {
    //     printhelp();
    //     return 1;
    // }

    // string InputFile = "";
    // string OutputFile = "";
    // string IRFile = "";
    // string VisualizationFile = "";
    // string OptimizeLevel = "";
    // for(int i = 1; i < argc; i++)
    // {
    //     string s(argv[i]);
    //     string tmp;
    //     if(s == "-i")
    //     {
    //         if(i + 1 <= argc - 1)
    //         {
    //             tmp = argv[i + 1];
    //             if(tmp[0] == '-')
    //             {
    //                 cout << "Fatal error: no input file." << endl;
    //                 printhelp();
    //                 return 1;
    //             }
    //             else
    //             {
    //                 if(tmp.length() < 2 || tmp.substr(tmp.size() - 2) != ".c")
    //                 {
    //                     cout << "There is a formatting error" << endl;
    //                     cout << "The source file is a C source file" << endl;
    //                     printhelp();
    //                     return 1;
    //                 }
    //                 else
    //                 {
    //                     InputFile = tmp;
    //                     i++;
    //                     continue;
    //                 }
    //             }
    //         }
    //         else {
    //             cout << "Fatal error: no input file." << endl;
    //             printhelp();
    //             return 1;
    //         }
    //     }
    //     else if(s == "-l")
    //     {
    //         if(!InputFile.empty())
    //         {
    //             auto it = InputFile.end();
    //             advance(it, -2);
    //             IRFile = InputFile.substr(0, distance(InputFile.begin(), it));
    //             IRFile += ".ll";
    //         }
    //         else {
    //             IRFile = "-";
    //         }
    //     }
    //     else if(s == "-v")
    //     {
    //         if(!InputFile.empty())
    //         {
    //             auto it = InputFile.end();
    //             advance(it, -2);
    //             VisualizationFile = InputFile.substr(0,
    //             distance(InputFile.begin(), it)); VisualizationFile += ".html";
    //         }
    //         else {
    //             VisualizationFile = "-";
    //         }
    //     }
    //     else if(s[0] == '-' && s[1] == 'O')
    //     {
    //         if(s == "-O0" || s == "-O1"|| s == "-O2" || s == "-O3"|| s == "-Oz"
    //         || s == "-Os")
    //         {
    //             OptimizeLevel = s;
    //         }
    //         else
    //         {
    //             cout << "Unsupported optimization level" << endl;
    //             printhelp();
    //             return 1;;
    //         }
    //     }
    //     else if(s == "-o")
    //     {
    //         if(i + 1 <= argc - 1)
    //         {
    //             tmp = argv[i + 1];
    //             if(tmp[0] == '-')
    //             {
    //                 OutputFile = "a.o";
    //             }
    //             else
    //             {
    //                 if(tmp.length() < 2 || tmp.substr(tmp.size() - 2) != ".o")
    //                 {
    //                     OutputFile = tmp + ".o";
    //                 }
    //                 else
    //                 {
    //                     OutputFile = tmp;
    //                 }
    //                 i++;
    //                 continue;
    //             }
    //         }
    //         else
    //         {
    //             OutputFile = "a.o";
    //         }
    //     }
    //     else
    //     {
    //         cout << "There is a formatting error" << endl;
    //         printhelp();
    //         return 1;
    //     }
    // }
    // if(OutputFile == "-")
    // {
    //     OutputFile = "a.o";
    // }
    // if(IRFile == "-")
    // {
    //     auto it = InputFile.end();
    //     advance(it, -2);
    //     IRFile = InputFile.substr(0, distance(InputFile.begin(), it));
    //     IRFile += ".ll";
    // }
    // if(VisualizationFile == "-")
    // {
    //     auto it = InputFile.end();
    //             advance(it, -2);
    //             VisualizationFile = InputFile.substr(0,
    //             distance(InputFile.begin(), it)); VisualizationFile += ".html";
    // }

    // yyin = fopen(InputFile.c_str(), "r");
    // assert(yyin);
    // std::cout << "lexer pass" << std::endl;
    // unique_ptr<BaseAST> ast;

    // auto ret = yyparse(ast);
    // assert(!ret);
    // std::cout << "passer pass" << std::endl;

    // ast->codegen();
    // IROptimize(OptimizeLevel);

    // GenerateObject(OutputFile);
    // std::error_code EC;
    // if(IRFile != "")
    // {
    //     //llvm::raw_fd_ostream ofs(output, EC);
    //     llvm::raw_fd_ostream ofs(IRFile, EC);
    //     IR::get()->getModule()->print(ofs, nullptr);
    // }
    // else
    // {
    //     (IR::get()->getModule())->print(llvm::outs(),
    //     nullptr);;//直接输出在终端
    // }

    // if(VisualizationFile != "")
    // {
    //     GenerateVisualization(ast, VisualizationFile);
    // }

    auto mode = argv[1];
    auto input = argv[2];
    auto output = argv[4];

    // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
    // yyin = fopen(input, "r");
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
    return 0 ;
    cout << endl;
    // visualization test
    extern const char *Html;
    std::string OutputString = Html;
    std::string Json = ast->astJson(1000);
    // std::cout << Json;
    std::string Target = "${ASTJson}";
    auto Pos = OutputString.find(Target);
    OutputString.replace(Pos, Target.length(), Json.c_str());
    std::ofstream HTMLFile("visualization.html");
    HTMLFile << OutputString;
    HTMLFile.close();
    // std::cout << Json;
    return 0;
}

void gep_test() {
    auto builder = IR::get()->getBuilder().get();
    auto F = IRGen::Function::Create(
            IRGen::FunctionType::get(IRGen::Type::getInt32()),
            IRGen::Function::ExternalLinkage, "main", IR::get()->getModule().get());
    IR::get()->EnterFunc(F);
    F = IRGen::Function::Create(IRGen::FunctionType::get(IRGen::Type::getInt32(),
                                                         {IRGen::Type::getPtr()}),
                                IRGen::Function::ExternalLinkage, "test",
                                IR::get()->getModule().get());

    IR::get()->NewLogicalBlockStart();
    auto inst = builder->CreateAlloca(
            IRGen::Type::getArray(IRGen::Type::getInt32(), 5), "array");
    IR::get()->AddAlloca(inst, "array");

    inst = builder->CreateGEP(IRGen::Type::getArray(IRGen::Type::getInt32(), 5),
                              IR::get()->GetAlloca("array"),
                              {IRGen::Constant::get(IRGen::Type::getInt32(), 0),
                               IRGen::Constant::get(IRGen::Type::getInt32(), 0)});
    builder->CreateStore(IRGen::Constant::get(IRGen::Type::getInt32(), 2233),
                         inst);
    builder->CreateCall(IR::get()->getModule()->get_function("test"), {inst});
    builder->CreateStore(IRGen::Constant::get(IRGen::Type::getInt32(), 3322),
                         inst);
    inst = builder->CreateLoad(inst);
    builder->CreateCall(IR::get()->getModule()->get_function("putint"), {inst});

    builder->CreateRet(IRGen::Constant::get(IRGen::Type::getInt32(), 1));
    IR::get()->IR::NewLogicalBlockEnd();

    auto argIter = F->arg_begin();
    argIter->get()->set_name("array");
    IR::get()->EnterFunc(F);
    IR::get()->NewLogicalBlockStart();
    {
        auto arg = builder->CreateAlloca(argIter->get()->get_type(),
                                         argIter->get()->get_name(), false);
        IR::get()->AddAlloca(arg, argIter->get()->get_name());
        builder->CreateStore(argIter->get(), arg);
    }
    {

        auto temp = IR::get()->GetAlloca("array");
        inst =
                builder->CreateGEP(IRGen::Type::getArray(IRGen::Type::getInt32(), 5),
                                   builder->CreateLoad(temp),
                                   {IRGen::Constant::get(IRGen::Type::getInt32(), 0),
                                    IRGen::Constant::get(IRGen::Type::getInt32(), 0)});

        inst = builder->CreateLoad(inst);
        builder->CreateCall(IR::get()->getModule()->get_function("putint"), {inst});

        builder->CreateRet(IRGen::Constant::get(IRGen::Type::getInt32(), 1));

        IR::get()->IR::NewLogicalBlockEnd();
    }

    std::ofstream file("demo.ll");
    file << IR::get()->getModule()->print().str();

    file.close();
}
