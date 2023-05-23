//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/BlockItemAST.h"

void BlockItemAST::Dump() const {

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
