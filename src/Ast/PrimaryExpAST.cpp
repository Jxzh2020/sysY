//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/PrimaryExpAST.h"
#include <string>
#include <vector>

std::string PrimaryExpAST::astJson(int size) {
    // std::unique_ptr<BaseAST> Exp;
    // std::unique_ptr<BaseAST> LVal;
    // std::unique_ptr<BaseAST> Number;
    if (Number != nullptr) {
        return Json("Number", {Number->astJson(sizeplus(size))}, size);
    }
        // ( Exp ) form
    else if (LVal == nullptr) {
        return Json("Expression", {Exp->astJson(sizeplus(size))}, size);
    } else {
        return Json("Load Value", {LVal->astJson(sizeplus(size))}, size);
    }
}

IRGen::IRBase *PrimaryExpAST::codegen() {
    IRGen::IRBase *res;
    // Number form
    if (Number != nullptr) {
        res = Number->codegen();
    } else if (LVal == nullptr) // ( Exp ) form
    {
        res = Exp->codegen();
    } else {
        res = LVal->codegen();
        res = IR::get()->getBuilder()->CreateLoad(res);
    }
    return res;
}
