//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/DeclAST.h"

void DeclAST::Dump() const {
    std::cout << "DeclAST { ";
    if (VarDecl == nullptr)
        ConstDecl->Dump();
    else
        VarDecl->Dump();
    std::cout << " }";
}

llvm::Value *DeclAST::codegen() {
    if (ConstDecl == nullptr)
        return VarDecl->codegen();
    else
        return ConstDecl->codegen();
}
