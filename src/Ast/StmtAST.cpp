//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/StmtAST.h"

void StmtAST::Dump() const {
    std::cout << "StmtAST { ";
    Exp->Dump();
    std::cout << " }";
}

llvm::Value *StmtAST::codegen() const {
    // so far, only return inst is supported.
    auto val = Exp->codegen();
    auto& builder = IR::get()->getBuilder();


    //auto ret = llvm::ReturnInst::Create(*IR::get()->getContext(),llvm::ConstantInt::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),0));
    //auto ret = llvm::ReturnInst::Create(*IR::get()->getContext(),val);
    // add to the end of the current block
    //IR::get()->getBuilder()->Insert(ret);
    llvm::Value* res;
    if(auto I = llvm::dyn_cast<llvm::AllocaInst>(val))
        res = IR::get()->getBuilder()->CreateLoad(I->getAllocatedType(),val);
    else
        res = val;

    builder->CreateRet(res);

    return nullptr;
}