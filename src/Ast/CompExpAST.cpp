//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/CompExpAST.h"
#include <llvm-14/llvm/IR/DerivedTypes.h>
#include <string>
#include <vector>

std::string CompExpAST::astJson(int size) {
    // CompOp type;
    // // if RHS == nullptr, it's not an exp of this level!
    // std::unique_ptr<BaseAST> LHS;
    // std::unique_ptr<BaseAST> RHS;
    std::vector<std::string> children;
    if (RHS == nullptr) {
        children.push_back(Json("Compare Left-Hand-Side", {LHS->astJson(sizeplus(size))}, sizeplus(size)));
    }
    else
    {
        std::string op;
        switch (type)
        {
            case LT:
                op = "<";
                break;
            case GT:
                op = ">";
                break;
            case LE:
                op = "<=";
                break;
            case GE:
                op = ">=";
                break;
            case EQ:
                op = "==";
                break;
            case NEQ:
                op = "!=";
                break;
        }
        children.push_back(Json("Compare Operation", {Escape(op)}, sizeplus(size)));
        children.push_back(Json("Left-Hand-Side", {LHS->astJson(sizeplus(size))}, sizeplus(size)));
        children.push_back(Json("Right-Hand-Side", {RHS->astJson(sizeplus(size))}, sizeplus(size)));
    }
    return Json("Compare Expression", children, size);
}

llvm::Value *CompExpAST::codegen() {
    llvm::Value *res;
    if (RHS == nullptr) {
        res = LHS->codegen();
        return res;
    }

    auto &builder = IR::get()->getBuilder();
    switch (type) {
        case LT:
            res = builder->CreateICmpSLT(LHS->codegen(), RHS->codegen());
            break;
        case GT:
            res = builder->CreateICmpSGT(LHS->codegen(), RHS->codegen());
            break;
        case LE:
            res = builder->CreateICmpSLE(LHS->codegen(), RHS->codegen());
            break;
        case GE:
            res = builder->CreateICmpSGE(LHS->codegen(), RHS->codegen());
            break;
        case EQ:
            res = builder->CreateICmpEQ(LHS->codegen(), RHS->codegen());
            break;
        case NEQ:
            res = builder->CreateICmpNE(LHS->codegen(), RHS->codegen());
            break;
    }
    return res;
}
