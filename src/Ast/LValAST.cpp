//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/LValAST.h"

void LValAST::Dump() const {

}

IRGen::IRBase *LValAST::codegen() {
    // TODO: unsolved constant assignment

    // 1. local variable, it seems that LLVM automatically shadows variables in nested logical blocks
    //auto ref = IR::get()->getFunc()->getValueSymbolTable()->lookup(ident);
    auto ref = IR::get()->GetAlloca(ident);
    // 2. function parameter
    if (ref == nullptr && !IR::get()->getFunc()->arg_empty()) {
        auto argIter = IR::get()->getFunc()->arg_begin();
        auto argEnd = IR::get()->getFunc()->arg_end();
        for (; argIter != argEnd; argIter++)
            if (argIter->get()->get_name() == ident) {
                ref = IRGen::IRBase::CreateIRBase(IRGen::IR_ARG, argIter->get());  //IR::get()->getBuilder()->CreateLoad(argIter->getType(),argIter);
                break;
            }
    }
    // 3. global variable
    if (ref == nullptr) {
        ref = IR::get()->getModule()->getGlobalVariable(ident);
    }
    // not found
    if (ref == nullptr) {
        std::cout << "Error! LVal not found!" << std::endl;
        exit(1);
    }
    //auto val = IR::get()->getBuilder()->CreateLoad(static_cast<IRGen::AllocaInst*>(ref)->getAllocatedType(),ref);
    //IRGen::AllocaInst* I = IRGen::dyn_cast<IRGen::AllocaInst>(val)
    //if(auto val = IRGen::dyn_cast<IRGen::AllocaInst>(ref))
    //    return IR::get()->getBuilder()->CreateLoad(val->getAllocatedType(),val);

    return ref;
}
