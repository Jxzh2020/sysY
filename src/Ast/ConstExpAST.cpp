//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstExpAST.h"
#include <string>
#include <vector>

std::string ConstExpAST::astJson(int size) {
    // std::unique_ptr<BaseAST> Exp;
    std::vector<std::string> children;
    children.push_back(Json("Expression", {Exp->astJson(sizeplus(size))}, sizeplus(size)));
    return Json("Constant Expression", children, size);
}

IRGen::IRBase *ConstExpAST::codegen() {
    return Exp->codegen();
}
