//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/AddExpAST.h"

void AddExpAST::Dump() const {
    std::cout << "AddExp { ";
    if (AddExp == nullptr) {
        MulExp->Dump();
    } else {
        AddExp->Dump();
        std::cout << (type == PLUS ? "PLUS " : "MINUS ");
        MulExp->Dump();
    }
    std::cout << " }";
}

IRGen::IRBase *AddExpAST::codegen() {
    if (AddExp == nullptr) {
        return MulExp->codegen();
    }

    IRGen::IRBase *res;
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
