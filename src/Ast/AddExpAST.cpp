//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/AddExpAST.h"
#include "Ast/BaseAST.h"
#include <string>
#include <vector>

std::string AddExpAST::astJson(int size) {
    // std::unique_ptr<BaseAST> MulExp;
    // std::unique_ptr<BaseAST> AddExp;
    // UnaryOp type;
    std::vector<std::string> children;
    if (AddExp == nullptr) {
        // children.push_back(Json("Mul Expression", {MulExp->astJson(sizeplus(size))}, sizeplus(size)));
        return MulExp->astJson(sizeplus(size));
    } else {
        std::string op = "";
        switch (type) {
            case PLUS:
                op = "+";
                break;
            case MINUS:
                op = "-";
                break;
            case COMPLEMENT:
                op = "~";
                break;
        }
        children.push_back(Json("op", {("{\"name\": "+Escape(op)+"}")}, sizeplus(size)));
        children.push_back(Json("LHS", {AddExp->astJson(sizeplus(size))}, sizeplus(size)));
        children.push_back(Json("RHS", {MulExp->astJson(sizeplus(size))}, sizeplus(size)));
    }
    return Json("Add Exp", children, size);
}

IRGen::IRBase *AddExpAST::codegen() {
    if (AddExp == nullptr) {
        return MulExp->codegen();
    }

    IRGen::IRBase *res;
    auto LHS = AddExp->codegen();
    auto RHS = MulExp->codegen();

    switch (type) {
        case PLUS:
            res = IR::get()->getBuilder()->CreateAdd(LHS, RHS);
            break;
        case MINUS:
            res = IR::get()->getBuilder()->CreateSub(LHS, RHS);
            break;
        default:
            res = nullptr;
    }
    return res;
}
