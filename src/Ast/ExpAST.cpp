//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/ExpAST.h"
#include "Ast/BaseAST.h"
#include <cstddef>
#include <string>
#include <vector>

std::string ExpAST::astJson(int size) {
      // std::unique_ptr<BaseAST> LgExp;
    std::vector<std::string> children;
      // children.push_back(Json("Logical Expression", {LgExp->astJson(sizeplus(size))}, sizeplus(size)));
    // return Json("Expression", {LgExp->astJson(sizeplus(size))}, size);
    if(LgExp!=NULL){
        return LgExp->astJson(size);
    }
    else {
        for (auto &exp: this->vals)
        {
            children.push_back(Json(exp->astJson(sizeplus(size)), sizeplus(size)));
        }
        return Json("Arr Exp",children, size);
    }
}

IRGen::IRBase *ExpAST::codegen() {
    // only one candidate, skip this node
    if (LgExp == nullptr)
        return nullptr;
    return LgExp->codegen();

}
