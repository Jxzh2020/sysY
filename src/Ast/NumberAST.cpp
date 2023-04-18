//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/NumberAST.h"

void NumberAST::Dump() const {
    std::cout << value;
}

llvm::Value *NumberAST::codegen() const {
    return nullptr;
}
