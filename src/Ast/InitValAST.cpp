//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/InitValAST.h"

std::string InitValAST::astJson(int size) {
    // std::unique_ptr<BaseAST> Exp;
    std::vector<std::string> children;
    children.push_back(Json("Expression", {Exp->astJson(sizeplus(size))}, sizeplus(size)));
    return Json("Init Value Expression", children, size);
}

IRGen::IRBase *InitValAST::codegen() {
    if(Exp)
        return Exp->codegen();
    return nullptr;
}
