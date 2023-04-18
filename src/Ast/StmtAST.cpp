//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/StmtAST.h"

void StmtAST::Dump() const {
    std::cout << "StmtAST { ";
    std::cout << *number ;
    std::cout << " }";
}

llvm::Value *StmtAST::codegen() const {
    return nullptr;
}