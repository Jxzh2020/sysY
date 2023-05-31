//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/VarDefAST.h"
#include "Ast/BaseAST.h"
#include <string>
#include <vector>

std::string VarDefAST::astJson(int size) {
    // std::string ident;
    // std::unique_ptr<BaseAST> ConstExp;
    // std::unique_ptr<BaseAST> InitVal;
    std::vector<std::string> children;
    children.push_back(Json("Variable", {Escape(ident)}, sizeplus(size)));
    if (InitVal != nullptr)
    {
        if(ConstExp!=nullptr)
        {
            children.push_back(Json("Array Index", {ConstExp->astJson(sizeplus(size))}, sizeplus(size)));
        }
        children.push_back(Json("Init Value", {InitVal->astJson(sizeplus(size))}, sizeplus(size)));
    }
    else if(ConstExp!=nullptr)
    {
        children.push_back(Json("Array Index", {ConstExp->astJson(sizeplus(size))}, sizeplus(size)));
    }
    return Json("Variable Define", children, size);
}

IRGen::IRBase *VarDefAST::codegen() {
    // -> IDENT
    IRGen::IRBase *res;
    auto &builder = IR::get()->getBuilder();
    if (InitVal == nullptr) { ;
    }
        // -> IDENT = InitVal
    else { ;
    }
    std::cout << "VarDefAST::codegen has been deprecated. Use VarDefAST::get_defs instead.\n";
    return nullptr;
}

std::pair<const std::string &, IRGen::IRBase *> VarDefAST::get_defs() const {
    // -> IDENT
    if (InitVal == nullptr) {
        return std::pair<const std::string &, IRGen::IRBase *>{ident, nullptr};
    }
        // -> IDENT = InitVal
    else {
        return std::pair<const std::string &, IRGen::IRBase *>{ident, InitVal->codegen()};
    }
    
}