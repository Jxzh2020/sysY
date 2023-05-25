//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/PrimaryExpAST.h"

void PrimaryExpAST::Dump() const {
    std::cout << "PrimaryExpAST { ";
    if (Exp != nullptr) {
        std::cout << "( ";
        Exp->Dump();
        std::cout << " )";
    } else if (LVal == nullptr) {
        Number->Dump();
    } else {
        LVal->Dump();
    }
    std::cout << " }";
}

IRGen::IRBase *PrimaryExpAST::codegen() {
    IRGen::IRBase *res;
    // Number form
    if (Number != nullptr) {
        res = Number->codegen();
    }
        // ( Exp ) form
    else if (LVal == nullptr) {
        res = Exp->codegen();
    } else {
        res = LVal->codegen();
        res = IR::get()->getBuilder()->CreateLoad(res);

    }
    return res;
}
