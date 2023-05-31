//
// Created by Hao Zhong on 4/26/23.
//

#include "Ast/FuncFParamAST.h"

std::string FuncFParamAST::astJson(int size) 
{
    // std::string ident;
    // std::unique_ptr<BaseAST> PrimitiveType;
    std::vector<std::string> children;

    children.push_back(Json("Type Name", {Escape(ident)}, sizeplus(size)));

    children.push_back(Json("Primitive Type", {PrimitiveType->astJson(sizeplus(size))}, sizeplus(size)));

    return Json("Function Parameter", children, size);
}

const std::string &FuncFParamAST::getName() const {
    return ident;
}

IRGen::Type *FuncFParamAST::getType() const {
    return dynamic_cast<PrimitiveTypeAST *>(PrimitiveType.get())->get_type();
}
