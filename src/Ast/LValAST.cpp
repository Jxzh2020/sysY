//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/LValAST.h"

void LValAST::Dump() const {

}

llvm::Value *LValAST::codegen() const {
    // TODO: unsolved constant assignment
    auto ref = IR::get()->GetFunc()->getValueSymbolTable()->lookup(ident);
    auto val = IR::get()->getBuilder()->CreateLoad(static_cast<llvm::AllocaInst*>(ref)->getAllocatedType(),ref);

    return val;
}
