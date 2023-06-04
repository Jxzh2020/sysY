//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/DeclAST.h"

std::string DeclAST::astJson(int size) {
    // std::unique_ptr<BaseAST> ConstDecl;
    // std::unique_ptr<BaseAST> VarDecl;
    std::vector<std::string> children;

    // if (ConstDecl != nullptr)
    //     children.push_back(Json("Const Declaration", {ConstDecl->astJson(sizeplus(size))}, sizeplus(size)));
    // else
    //     children.push_back(Json("Variable Declaration", {VarDecl->astJson(sizeplus(size))}, sizeplus(size)));

    // return Json("Declaration", children, size);
    if (ConstDecl != nullptr)
        return ConstDecl->astJson(size);
    else
        return VarDecl->astJson(size);
}

IRGen::IRBase *DeclAST::codegen() {
    if (ConstDecl == nullptr)
        return VarDecl->codegen();
    else
        return ConstDecl->codegen();
}
