//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/BlockItemAST.h"

void BlockItemAST::Dump() const {

}

llvm::Value *BlockItemAST::codegen() const {
    llvm::Value* res;
    if(Decl == nullptr)
        res = Stmt->codegen();
    else
        res = Decl->codegen();
    return res;
}
