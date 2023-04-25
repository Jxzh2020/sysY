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
    auto tmp = dynamic_cast<FunctypeAST*>(func_type.get())->get_type();    //llvm::FunctionType::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),false);
    auto F = llvm::Function::Create(tmp,llvm::Function::ExternalLinkage,this->ident,IR::get()->getModule().get());
    if(block != nullptr){
        IR::get()->EnterFunc(F);
        IR::get()->NewLogicalBlockStart();
        block->codegen();
        IR::get()->NewLogicalBlockEnd();
    }
    // TODO Function Declaration
    else{
        ;
    }

//    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*IR::get()->getContext(), "entry", F);
//    llvm::ReturnInst::Create(*IR::get()->getContext(),llvm::ConstantInt::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),0),BB);
    return F;
}