//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/FunctypeAST.h"

void FunctypeAST::Dump() const {
    std::cout << "FunctypeAST { ";
    std::cout << type;
    std::cout << " }";
}

llvm::Value *FunctypeAST::codegen() const {

    return nullptr;
}