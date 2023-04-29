//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/LgExpAST.h"

void LgExpAST::Dump() const {

}

llvm::Value *LgExpAST::codegen() {
    llvm::Value *res;
    auto &builder = IR::get()->getBuilder();
    if (RHS == nullptr) {
        res = LHS->codegen();
        return res;
    }
    switch (type) {
        case OR:
            res = builder->CreateLogicalOr(LHS->codegen(), RHS->codegen());
            break;
        case AND:
            res = builder->CreateLogicalAnd(LHS->codegen(), RHS->codegen());
            break;
    }
    return res;
}
