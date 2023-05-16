//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/MulExpAST.h"

std::string MulExpAST::astJson() {
    std::cout << "MulExp { ";
    if (MulExp == nullptr) {
        UnaryExp->astJson();
    } else {
        MulExp->astJson();
        std::cout << (type == MUL ? "MUL " : type == DIV ? "DIV " : "MOD ");
        UnaryExp->astJson();
    }
    std::cout << " }";
}

llvm::Value *MulExpAST::codegen() {
    if (MulExp == nullptr) {
        return UnaryExp->codegen();
    }

    auto LHS = MulExp->codegen();
    auto RHS = UnaryExp->codegen();
    llvm::Value *res;

    switch (type) {
        case MUL:
            res = IR::get()->getBuilder()->CreateMul(LHS, RHS);
            break;
        case DIV:
            res = IR::get()->getBuilder()->CreateSDiv(LHS, RHS);
            break;
        case MOD:
            res = IR::get()->getBuilder()->CreateSRem(LHS, RHS);;
            break;
    }
    return res;
}
