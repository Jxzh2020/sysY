//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/ExpAST.h"

std::string ExpAST::astJson() {
    std::cout << "ExpAST { ";
    LgExp->astJson();
    std::cout << " }";
}

llvm::Value *ExpAST::codegen() {
    // only one candidate, skip this node
    return LgExp->codegen();
}
