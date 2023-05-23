//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/NumberAST.h"

void NumberAST::Dump() const {
    std::cout << value;
}

IRGen::IRBase *NumberAST::codegen() {
    return IRGen::Constant::get(IRGen::Type::getInt32(), value);
}
