//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/FuncDefAST.h"

void FuncDefAST::Dump() const {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
}

llvm::Value *FuncDefAST::codegen() const {
    auto tmp = llvm::FunctionType::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),false);
    auto F = llvm::Function::Create(tmp,llvm::Function::ExternalLinkage,this->ident,IR::get()->getModule().get());
    IR::get()->EnterFunc(F);
    block->codegen();
//    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*IR::get()->getContext(), "entry", F);
//    llvm::ReturnInst::Create(*IR::get()->getContext(),llvm::ConstantInt::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),0),BB);
    return F;
}