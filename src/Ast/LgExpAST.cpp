//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/LgExpAST.h"

std::string LgExpAST::astJson(int size) {
    // LogicOp type;
    // // if RHS == nullptr, it's not an exp of this level!
    // std::unique_ptr<BaseAST> LHS;
    // std::unique_ptr<BaseAST> RHS;
    std::vector<std::string> children;
    if (RHS == nullptr) {
        children.push_back(Json("Logical Left-Hand-Side", {LHS->astJson(sizeplus(size))}, sizeplus(size)));
    }
    else
    {
        std::string op;
        switch (type)
        {
            case OR:
                op = "||";
                break;
            case AND:
                op = "&&";
                break;
        }
        children.push_back(Json("Logical Operation", {Escape(op)}, sizeplus(size)));
        children.push_back(Json("Left-Hand_Side", {LHS->astJson(sizeplus(size))}, sizeplus(size)));
        children.push_back(Json("Right-Hand-Side", {RHS->astJson(sizeplus(size))}, sizeplus(size)));
    }
    return Json("Logical Expression", children, size);
}

llvm::Value *LgExpAST::codegen() {
    llvm::Value *res;
    auto &builder = IR::get()->getBuilder();
    if (RHS == nullptr) {
        res = LHS->codegen();
    }
    else{
        switch (type) {
            case OR:
                res = builder->CreateLogicalOr(LHS->codegen(), RHS->codegen());
                break;
            case AND:
                res = builder->CreateLogicalAnd(LHS->codegen(), RHS->codegen());
                break;
        }
    }
    return res;
}
