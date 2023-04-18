//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/BlockAST.h"

void BlockAST::Dump() const {
    std::cout << "BlockAST { ";
    stmt->Dump();
    std::cout << " }";
}

llvm::Value *BlockAST::codegen() const {
    return nullptr;
}
