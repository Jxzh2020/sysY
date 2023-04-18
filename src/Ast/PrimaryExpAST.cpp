//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/PrimaryExpAST.h"

void PrimaryExpAST::Dump() const {
    std::cout << "PrimaryExpAST { ";
    if( Exp == nullptr)
        Number->Dump();
    else{
        std::cout << "( ";
        Exp->Dump();
        std::cout << " )";
    }
    std::cout << " }";
}

llvm::Value *PrimaryExpAST::codegen() const {
    return nullptr;
}
