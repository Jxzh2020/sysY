//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/DeclAST.h"

std::string DeclAST::astJson() {
    std::cout << "DeclAST { ";
    if (VarDecl == nullptr)
        ConstDecl->astJson();
    else
        VarDecl->astJson();
    std::cout << " }";
}

llvm::Value *DeclAST::codegen() {
    if (ConstDecl == nullptr)
        return VarDecl->codegen();
    else
        return ConstDecl->codegen();
}
