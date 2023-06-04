//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/LgExpAST.h"
#include "Ast/BaseAST.h"

std::string LgExpAST::astJson(int size) {
    // LogicOp type;
    // // if RHS == nullptr, it's not an exp of this level!
    // std::unique_ptr<BaseAST> LHS;
    // std::unique_ptr<BaseAST> RHS;
    std::vector<std::string> children;
    if (RHS == nullptr) {
        // children.push_back(Json("Left-Hand-Side", LHS->astJson(sizeplus(size)), sizeplus(size)));
        // children.push_back(Json(LHS->astJson(sizeplus(size)), sizeplus(size)));
        // return Json("Left Hand Side", LHS->astJson(sizeplus(size)), sizeplus(size));
        return LHS->astJson(sizeplus(size));
    } else {
        std::string op;
        switch (type) {
            case OR:
                op = "||";
                break;
            case AND:
                op = "&&";
                break;
        }
        children.push_back(Json("Lg Op", {("{\"name\": "+Escape(op)+"}")}, sizeplus(size)));
        children.push_back(Json("LHS", {LHS->astJson(sizeplus(size))}, sizeplus(size)));
        children.push_back(Json("RHS", {RHS->astJson(sizeplus(size))}, sizeplus(size)));
    }
    return Json("Lg Exp", children, size);
}

IRGen::IRBase *LgExpAST::codegen() {
    IRGen::IRBase *res;
    auto &builder = IR::get()->getBuilder();
    if (RHS == nullptr) {
        res = LHS->codegen();
    } else {
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
