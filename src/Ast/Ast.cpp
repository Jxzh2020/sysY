//
// Created by Hao Zhong on 4/17/23.
//

#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include "Ast/Ast.h"
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
static std::unique_ptr<llvm::Module> TheModule;
static std::map<std::string, llvm::Value *> NamedValues;

static void InitializeModule() {
    // Open a new context and module.
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("Jxzh2020_sysY", *TheContext);

    // Create a new builder for the module.
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

std::unique_ptr<BaseAST> LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
    LogError(Str);
    return nullptr;
}

void CompUnitAST::Dump() const {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }" << std::endl;

    InitializeModule();

    this->codegen();
}
llvm::Value* CompUnitAST::codegen() const{
    this->func_def->codegen();
    llvm::raw_ostream &OS = llvm::outs();
    TheModule->print(OS,nullptr);
    return nullptr;
}

void FuncDefAST::Dump() const {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
}

llvm::Value *FuncDefAST::codegen() const {
    auto tmp = llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext),false);
    auto F = llvm::Function::Create(tmp,llvm::Function::ExternalLinkage,this->ident,TheModule.get());

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, "entry", F);
    llvm::ReturnInst::Create(*TheContext,llvm::ConstantInt::get(llvm::Type::getInt32Ty(*TheContext),0),BB);
    return F;
}

void FunctypeAST::Dump() const {
    std::cout << "FunctypeAST { ";
    std::cout << type;
    std::cout << " }";
}

llvm::Value *FunctypeAST::codegen() const {

    return nullptr;
}

void BlockAST::Dump() const {
    std::cout << "BlockAST { ";
    stmt->Dump();
    std::cout << " }";
}

llvm::Value *BlockAST::codegen() const {
    return nullptr;
}

void StmtAST::Dump() const {
    std::cout << "StmtAST { ";
    std::cout << *number ;
    std::cout << " }";
}

llvm::Value *StmtAST::codegen() const {
    return nullptr;
}

