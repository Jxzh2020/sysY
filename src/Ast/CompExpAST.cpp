//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/CompExpAST.h"

void CompExpAST::Dump() const {

}

llvm::Value *CompExpAST::codegen() const {
    llvm::Value *res;
    if (RHS == nullptr) {
        res = LHS->codegen();
        return res;
    }

    auto &builder = IR::get()->getBuilder();
    switch (type) {
        case LT:
            res = builder->CreateICmpSLT(LHS->codegen(), RHS->codegen());
            break;
        case GT:
            res = builder->CreateICmpSGT(LHS->codegen(), RHS->codegen());
            break;
        case LE:
            res = builder->CreateICmpSLE(LHS->codegen(), RHS->codegen());
            break;
        case GE:
            res = builder->CreateICmpSGE(LHS->codegen(), RHS->codegen());
            break;
        case EQ:
            res = builder->CreateICmpEQ(LHS->codegen(), RHS->codegen());
            break;
        case NEQ:
            res = builder->CreateICmpNE(LHS->codegen(), RHS->codegen());
            break;
    }
    return res;
}
