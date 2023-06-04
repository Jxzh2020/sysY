//
// Created by Hao Zhong on 4/20/23.
//

#include "Ast/MulExpAST.h"
#include "Ast/BaseAST.h"
#include <string>
#include <vector>

std::string MulExpAST::astJson(int size) {
    // std::unique_ptr<BaseAST> UnaryExp;
    // std::unique_ptr<BaseAST> MulExp;
    // BinaryOp type;
    std::vector<std::string> children;
    std::string op = "";
    if (MulExp == nullptr) {
        // children.push_back(Json("Unary Expression", {UnaryExp->astJson(sizeplus(size))}, sizeplus(size)));
        return UnaryExp->astJson(sizeplus(size));
    } else {
        switch (type) {
            case MUL:
                op = "*";
                break;
            case DIV:
                op = "/";
                break;
            case MOD:
                op = "%";
                break;
        }
        children.push_back(Json("LHS", {MulExp->astJson(sizeplus(size))}, sizeplus(size)));
        children.push_back(Json("RHS", {UnaryExp->astJson(sizeplus(size))}, sizeplus(size)));
        children.push_back(Json("Mul Type", {("{\"name\": "+Escape(op)+"}")}, size));
    }
    return Json("Mul Exp", children, size);
}

IRGen::IRBase *MulExpAST::codegen() {
    if (MulExp == nullptr) {
        return UnaryExp->codegen();
    }

    auto LHS = MulExp->codegen();
    auto RHS = UnaryExp->codegen();
    IRGen::IRBase *res;

    switch (type) {
        case MUL:
            res = IR::get()->getBuilder()->CreateMul(LHS, RHS);
            break;
        case DIV:
            res = IR::get()->getBuilder()->CreateSDiv(LHS, RHS);
            break;
        case MOD:
            res = IR::get()->getBuilder()->CreateSRem(LHS, RHS);;
            break;
    }
    return res;
}
