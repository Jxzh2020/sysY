//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstDefAST.h"
#include "Ast/BaseAST.h"
#include <string>
#include <vector>

std::string ConstDefAST::astJson(int size) {
    // std::string ident;
    // std::unique_ptr<BaseAST> ConstInitVal;
    std::vector<std::string> children;
    children.push_back(Json("Constant", {Escape(ident)}, sizeplus(size)));
    children.push_back(Json("Constant Init", {ConstInitVal->astJson(sizeplus(size))}, sizeplus(size)));
    return Json("Constant Definition", children, size);
}

llvm::Value *ConstDefAST::codegen() {
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
