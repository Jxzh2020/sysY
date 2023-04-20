//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/NumberAST.h"

void NumberAST::Dump() const {
    std::cout << value;
}

llvm::Value *NumberAST::codegen() const {
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),value);
}