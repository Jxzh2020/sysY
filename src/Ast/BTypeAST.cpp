//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/BTypeAST.h"

void BTypeAST::Dump() const {
    std::cout << "BTypeAST { ";
    std::cout << type;
    std::cout << " }";
}

llvm::Value *BTypeAST::codegen() const {
    return nullptr;
}

llvm::Type *BTypeAST::get_type() const {
    if(type == "int")
        return llvm::Type::getInt32Ty(*IR::get()->getContext());
    std::cout << "BType Error!" << std::endl;
    return nullptr;
}
