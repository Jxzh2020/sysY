//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/ExpAST.h"

void ExpAST::Dump() const {
    std::cout << "ExpAST { ";
    UnaryExp->Dump();
    std::cout << " }";
}

llvm::Value *ExpAST::codegen() const {
    // only one candidate, skip this node
    return UnaryExp->codegen();
}
