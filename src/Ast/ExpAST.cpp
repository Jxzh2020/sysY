//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/ExpAST.h"
#include <string>
#include <vector>

std::string ExpAST::astJson(int size) {
    // std::unique_ptr<BaseAST> LgExp;
    std::vector<std::string> children;
    children.push_back(Json("Logical Expression", {LgExp->astJson(sizeplus(size))}, sizeplus(size)));
    return Json("Expression", children, size);
}

llvm::Value *ExpAST::codegen() {
    // only one candidate, skip this node
    return LgExp->codegen();
}
