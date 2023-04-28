//
// Created by Hao Zhong on 4/26/23.
//

#include "Ast/FuncFParamAST.h"

const std::string &FuncFParamAST::getName() const {
    return ident;
}

llvm::Type *FuncFParamAST::getType() const {
    return dynamic_cast<PrimitiveTypeAST*>(PrimitiveType.get())->get_type();
}
