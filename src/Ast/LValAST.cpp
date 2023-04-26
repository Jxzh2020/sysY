//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/LValAST.h"

void LValAST::Dump() const {

}

llvm::Value *LValAST::codegen() const {
    // TODO: unsolved constant assignment

    // 1. local variable, it seems that LLVM automatically shadows variables in nested logical blocks
    //auto ref = IR::get()->getFunc()->getValueSymbolTable()->lookup(ident);
    auto ref = IR::get()->GetAlloca(ident);
    // 2. function parameter
    if(ref == nullptr && !IR::get()->getFunc()->arg_empty()){
        auto argIter = IR::get()->getFunc()->arg_begin();
        auto argEnd = IR::get()->getFunc()->arg_end();
        for( ; argIter != argEnd; argIter++)
            if(argIter->getName().str() == ident){
                ref = argIter;  //IR::get()->getBuilder()->CreateLoad(argIter->getType(),argIter);
                break;
            }
    }
    // 3. global variable
    if(ref == nullptr){
        ref = IR::get()->getModule()->getGlobalVariable(ident);
    }
    // not found
    if(ref == nullptr){
        std::cout << "Error! LVal not found!" << std::endl;
        exit(1);
    }
    //auto val = IR::get()->getBuilder()->CreateLoad(static_cast<llvm::AllocaInst*>(ref)->getAllocatedType(),ref);
    //llvm::AllocaInst* I = llvm::dyn_cast<llvm::AllocaInst>(val)
    //if(auto val = llvm::dyn_cast<llvm::AllocaInst>(ref))
    //    return IR::get()->getBuilder()->CreateLoad(val->getAllocatedType(),val);

    return ref;
}
