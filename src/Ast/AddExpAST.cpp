//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/AddExpAST.h"

std::string AddExpAST::astJson() {
    std::cout << "AddExp { ";
    if (AddExp == nullptr) {
        MulExp->astJson();
    } else {
        AddExp->astJson();
        std::cout << (type == PLUS ? "PLUS " : "MINUS ");
        MulExp->astJson();
    }
    std::cout << " }";
}

llvm::Value *AddExpAST::codegen() {
    if (AddExp == nullptr) {
        return MulExp->codegen();
    }

    llvm::Value *res;
    auto LHS = AddExp->codegen();
    auto RHS = MulExp->codegen();

    switch (type) {
        case PLUS:
            res = IR::get()->getBuilder()->CreateAdd(LHS, RHS);
            break;
        case MINUS:
            res = IR::get()->getBuilder()->CreateSub(LHS, RHS);
            break;
        default:
            res = nullptr;
    }
    return res;
}
