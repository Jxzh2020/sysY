//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/NumberAST.h"
#include "Ast/BaseAST.h"

std::string NumberAST::astJson(int size) {
    // int value;
    return Json("Number", {Escape(std::to_string(value))}, size);
}

IRGen::IRBase *NumberAST::codegen() {
    return IRGen::Constant::get(IRGen::Type::getInt32(), value);
}
