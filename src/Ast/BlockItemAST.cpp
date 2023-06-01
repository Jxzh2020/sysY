//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/BlockItemAST.h"

std::string BlockItemAST::astJson(int size) {
    // std::unique_ptr<BaseAST> Decl;
    // std::unique_ptr<BaseAST> Stmt;
    std::vector<std::string> children;

    if (Decl != nullptr)
        children.push_back(Json("Declaration", {Decl->astJson(sizeplus(size))}, sizeplus(size)));
    else
        children.push_back(Json("Statement", {Stmt->astJson(sizeplus(size))}, sizeplus(size)));

    return Json("Block Item", children, size);
}

IRGen::IRBase *BlockItemAST::codegen() {
    IRGen::IRBase *res;
    if (Decl == nullptr)
        res = Stmt->codegen();
    else
        res = Decl->codegen();
    return res;
}

bool BlockItemAST::isBranch() const {
    if (Stmt == nullptr)
        return false;
    else
        return dynamic_cast<StmtAST *>(Stmt.get())->isBranch();
}
