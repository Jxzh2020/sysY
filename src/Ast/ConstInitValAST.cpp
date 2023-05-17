//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstInitValAST.h"
#include <string>
#include <vector>

std::string ConstInitValAST::astJson(int size) {
    // std::unique_ptr<BaseAST> ConstExp;
    std::vector<std::string> children;
    children.push_back(Json("Constant Expression", {ConstExp->astJson(sizeplus(size))}, sizeplus(size)));
    return Json("Constant Init", children, size);
}

llvm::Value *ConstInitValAST::codegen() {
    return ConstExp->codegen();
}
