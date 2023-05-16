//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/NumberAST.h"

std::string NumberAST::astJson() {
    std::cout << value;
}

llvm::Value *NumberAST::codegen() {
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*IR::get()->getContext()), value);
}
