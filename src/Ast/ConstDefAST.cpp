//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstDefAST.h"

void ConstDefAST::Dump() const {

}

llvm::Value *ConstDefAST::codegen() const {
    // deleted
    auto &builder = IR::get()->getBuilder();
    auto down = ConstInitVal->codegen();
    // TODO: unknown_usage
    auto res = builder->CreateAlloca(down->getType(), nullptr, ident);
    builder->CreateStore(down, res);
    return res;
}

std::pair<const std::string &, llvm::Value *> ConstDefAST::get_defs() const {
    auto down = ConstInitVal->codegen();
    return std::pair<const std::string &, llvm::Value *>{ident, down};
}
