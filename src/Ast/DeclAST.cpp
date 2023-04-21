//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/DeclAST.h"

void DeclAST::Dump() const {
    std::cout << "DeclAST { ";
    ConstDecl->Dump();
    std::cout << " }";
}

llvm::Value *DeclAST::codegen() const {
    return ConstDecl->codegen();
}
