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
#include <cstdlib>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/PassRegistry.h"

#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/TargetParser.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/CodeGen/CommandFlags.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/CodeGen.h>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Transforms/Scalar.h"

#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/Scalar.h>

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Passes/PassBuilder.h"


// #define GEP_TEST
using namespace std;
// using namespace llvm;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段

extern FILE *yyin;

extern int yyparse(unique_ptr<BaseAST> &ast);

void printhelp() {
  cout << "How to use MySysY compiler:" << endl;
  cout << "-help: print this message" << endl;
  cout << "-i: A MUST ARGUMENT, source code .c file." << endl;
  cout << "-o: generate output file, which is a .o file. If you ignore this argument, the output file will be named a.o" << endl;
  cout << "-l: generate llvm ir code file, which is a .ll file. If you ignore this argument, llvm ir will be print in the console"<< endl;
  cout << "-v: generate a HTML visualization file, which is a .html file, in which the syntax tree structure are shown. If you ignore this argument, the HTML file will not generate" << endl;
  cout << "-O: the level of optimization, supported levels: -O0, -O1, -O2, -O3. If you ignore this argument, it will not be optimized" << endl;
  cout << "For a instance: " << endl;
  cout << "If you want to compile source file test.c, get the output file out.o, get the llvm ir code file ir.ll, get the visualization file " "visual.html and optimize program in level O2"
       << endl;
  cout << "You should use command line: ./SysY -i test.c -o out.o -l -v -O2"
       << endl;
  return;
}

void GenerateVisualization(unique_ptr<BaseAST> &ast, string VisualizationFile) {
  // visualization test
  extern const char *Html;
  std::string OutputString = Html;
  std::string Json = ast->astJson(1000);
  // std::cout << Json;
  std::string Target = "${ASTJson}";
  auto Pos = OutputString.find(Target);
  OutputString.replace(Pos, Target.length(), Json.c_str());
  std::ofstream HTMLFile(VisualizationFile);
  HTMLFile << OutputString;
  HTMLFile.close();
}

bool GenerateObject(string OutputFile, string IRFile) {
    llvm::LLVMContext llvm_context;
    llvm::SMDiagnostic error;
    llvm::SourceMgr source_mgr;
    source_mgr.setDiagHandler([](const llvm::SMDiagnostic& diag, void* ctx) {});

    auto module = llvm::parseAssemblyFile(IRFile, error, llvm_context);
    std::cout << error.getMessage().str() << std::endl;

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string target_triple = llvm::Triple(module->getTargetTriple()).normalize();
    std::string error_string;

    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error_string);
    if (!target) {
        return false;
        // Handle error
    }
    llvm::CodeGenOpt::Level opt_level = llvm::CodeGenOpt::Default;

    auto cpu = "";
    auto features = "";

    llvm::TargetOptions opt;
    opt.MCOptions.AsmVerbose = true;
    opt.MCOptions.PreserveAsmComments = true;

    std::unique_ptr<llvm::TargetMachine> tm(target->createTargetMachine(target_triple, cpu, features, opt, llvm::None));

    std::error_code error_code;
    llvm::raw_fd_ostream output_file(OutputFile, error_code, llvm::sys::fs::OF_None);

    if (error_code) {
        return false;
        // Handle error
    }
    llvm::legacy::PassManager pass_manager;
    tm->addPassesToEmitFile(pass_manager, output_file, nullptr, llvm::CGFT_AssemblyFile);//static_cast<llvm::CodeGenFileType>(0));
    pass_manager.run(*module);
    output_file.flush();

    auto engineBuilder = llvm::EngineBuilder(std::move(module));
    engineBuilder.setTargetOptions(opt);
    engineBuilder.setEngineKind(llvm::EngineKind::JIT);

    auto executionEngine = engineBuilder.create(tm.get());
    executionEngine->finalizeObject();

    llvm::raw_fd_ostream outputStream("output.bin", error_code, llvm::sys::fs::OF_None);

    llvm::WriteBitcodeToFile(*module, outputStream);

    module.release();
    return true;
}

unsigned getOptimizationLevel(const std::string &OptimizeLevel) {
  if (OptimizeLevel == "-O0" || OptimizeLevel.empty()) {
    return 0;
  }else if (OptimizeLevel == "-O1") {
    return 1;
  } else if (OptimizeLevel == "-O2") {
    return 2;
  } else if (OptimizeLevel == "-O3") {
    return 3;
  } else {
    llvm::errs() << "Unknown optimization level: " << OptimizeLevel << "\n";
    exit(1);
  }
}

void IROptimize(string IRFile, string OptimizeLevel){
  // Create a module from the input IR file.
  llvm::SMDiagnostic Err;
  llvm::LLVMContext Context;
  auto Module = parseIRFile(IRFile, Err, Context);
  if (!Module) {
    Err.print("IROptimize", llvm::errs());
    return;
  }

  llvm::PassBuilder PB;
  llvm::LoopAnalysisManager LAM;
  llvm::FunctionAnalysisManager FAM;
  llvm::CGSCCAnalysisManager CGAM;
  llvm::ModuleAnalysisManager MAM;
  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  llvm::PassManagerBuilder PMBuilder;
  PMBuilder.OptLevel = getOptimizationLevel(OptimizeLevel);

  llvm::legacy::PassManager PM;
  PMBuilder.populateModulePassManager(PM);
  PM.run(*Module);

  std::error_code EC;
  llvm::raw_fd_ostream OS(IRFile, EC, llvm::sys::fs::OF_None);
  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message() << "\n";
    return;
  }
  llvm::WriteBitcodeToFile(*Module, OS);
}


int main(int argc, const char *argv[])
{
    bool help = false;
    for(int i = 1; i < argc; i++)
    {
        string s(argv[i]);
        if(s=="-help")
        {
            help = true;
            break;
        }
    }
    if (help || argc == 1)
    {
        printhelp();
        return 1;
    }

    string InputFile = "";
    string OutputFile = "";
    string IRFile = "";
    string VisualizationFile = "";
    string OptimizeLevel = "";
    for(int i = 1; i < argc; i++)
    {
        string s(argv[i]);
        string tmp;
        if(s == "-i")
        {
            if(i + 1 <= argc - 1)
            {
                tmp = argv[i + 1];
                if(tmp[0] == '-')
                {
                    cout << "Fatal error: no input file." << endl;
                    printhelp();
                    return 1;
                }
                else
                {
                    if(tmp.length() < 2 || tmp.substr(tmp.size() - 2) != ".c")
                    {
                        cout << "There is a formatting error" << endl;
                        cout << "The source file is a C source file" << endl;
                        printhelp();
                        return 1;
                    }
                    else
                    {
                        InputFile = tmp;
                        i++;
                        continue;
                    }
                }
            }
            else {
                cout << "Fatal error: no input file." << endl;
                printhelp();
                return 1;
            }
        }
        else if(s == "-l")
        {
            if(!InputFile.empty())
            {
                auto it = InputFile.end();
                advance(it, -2);
                IRFile = InputFile.substr(0, distance(InputFile.begin(), it));
                IRFile += ".ll";
            }
            else {
                IRFile = "-";
            }
        }
        else if(s == "-v")
        {
            if(!InputFile.empty())
            {
                auto it = InputFile.end();
                advance(it, -2);
                VisualizationFile = InputFile.substr(0,
                distance(InputFile.begin(), it)); VisualizationFile += ".html";
            }
            else {
                VisualizationFile = "-";
            }
        }
        else if(s[0] == '-' && s[1] == 'O')
        {
            if(s == "-O0" || s == "-O1"|| s == "-O2" || s == "-O3"|| s == "-Oz"
            || s == "-Os")
            {
                OptimizeLevel = s;
            }
            else
            {
                cout << "Unsupported optimization level" << endl;
                printhelp();
                return 1;;
            }
        }
        else if(s == "-o")
        {
            if(i + 1 <= argc - 1)
            {
                tmp = argv[i + 1];
                if(tmp[0] == '-')
                {
                    OutputFile = "a.o";
                }
                else
                {
                    if(tmp.length() < 2 || tmp.substr(tmp.size() - 2) != ".o")
                    {
                        OutputFile = tmp + ".o";
                    }
                    else
                    {
                        OutputFile = tmp;
                    }
                    i++;
                    continue;
                }
            }
            else
            {
                OutputFile = "a.o";
            }
        }
        else
        {
            cout << "There is a formatting error" << endl;
            printhelp();
            return 1;
        }
    }
    if(OutputFile == "-")
    {
        OutputFile = "a.o";
    }
    if(IRFile == "-")
    {
        auto it = InputFile.end();
        advance(it, -2);
        IRFile = InputFile.substr(0, distance(InputFile.begin(), it));
        IRFile += ".ll";
    }
    if(VisualizationFile == "-")
    {
        auto it = InputFile.end();
                advance(it, -2);
                VisualizationFile = InputFile.substr(0,
                distance(InputFile.begin(), it)); VisualizationFile += ".html";
    }

    yyin = fopen(InputFile.c_str(), "r");
    assert(yyin);
    std::cout << "lexer pass" << std::endl;
    unique_ptr<BaseAST> ast;

    auto ret = yyparse(ast);
    assert(!ret);
    std::cout << "passer pass" << std::endl;

    ast->codegen();

    if(VisualizationFile != "")
    {
        GenerateVisualization(ast, VisualizationFile);
    }

    std::ofstream file(IRFile);
    if(IRFile != "")
    {
        file << IR::get()->getModule()->print().str();
        file.close();
    }
    else
    {
        // (IR::get()->getModule())->print(llvm::outs(), nullptr);;//直接输出在终端
        std::cout << IR::get()->getModule()->print().str();
    }

    // IROptimize(IRFile, OptimizeLevel);

    ifstream file1("lib.ll");
    ifstream file2(IRFile);
    ofstream file3("Combined.ll");
    if (!file1.is_open()) {
        return 1;
    }
    if (!file2.is_open()) {
        return 1;
    }
    if (!file3.is_open()) {
        return 1;
    }
    // 从第一个文件中读取内容并写入新文件
    std::string line;
    while (std::getline(file1, line)) {
        file3 << line << '\n';
    }

    // 从第二个文件中读取内容并写入新文件
    int i = 0;
    while (std::getline(file2, line)) {
        if(i < 4)
        {
            i++;
        }
        else {
            file3 << line << '\n';
        }
    }

    // 关闭文件流
    file1.close();
    file2.close();
    file3.close();
    // if(GenerateObject(OutputFile, "Combined.ll"))
    // {
    //     cout << "object file generated successfully." << endl;
    // }
    // else
    // {
    //     cout << "object file generated failed." << endl;
    // }
    IROptimize("Combined.ll", OptimizeLevel);

    int res = system(std::string("clang Combined.ll -o "+ OutputFile /*+" "+OptimizeLevel*/).c_str());
    if(res != 0)
    {
        cout << "Error command" << endl;
    }

    return 0;
}