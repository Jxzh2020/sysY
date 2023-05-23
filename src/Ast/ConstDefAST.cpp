//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstDefAST.h"

void ConstDefAST::Dump() const {

}

IRGen::IRBase *ConstDefAST::codegen() {
    // deleted
    auto &builder = IR::get()->getBuilder();
    auto down = ConstInitVal->codegen();
    // TODO: unknown_usage
    auto res = builder->CreateAlloca(down->get_type(), ident);
    builder->CreateStore(down, res);
    return res;
}

std::pair<const std::string &, IRGen::IRBase *> ConstDefAST::get_defs() const {
    auto down = ConstInitVal->codegen();
    return std::pair<const std::string &, IRGen::IRBase *>{ident, down};
}
