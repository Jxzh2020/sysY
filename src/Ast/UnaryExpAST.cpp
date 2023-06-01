//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/UnaryExpAST.h"
#include "Ast/BaseAST.h"
#include <string>
#include <vector>

std::string UnaryExpAST::astJson(int size) {
    // std::unique_ptr<BaseAST> PrimaryExp;
    // std::unique_ptr<BaseAST> UnaryExp;
    // UnaryOp OpType;
    // std::vector<std::unique_ptr<BaseAST>> params;
    // std::string ident;
    std::vector<std::string> children;
    std::string op = "";
    if (ident.empty()) {  // still exp, not a function call
        // UnaryOp UnaryExp form
        if (PrimaryExp == nullptr) {
            switch (OpType) {
                case PLUS:
                    op = "+";
                    children.push_back(Json("PLUS", {UnaryExp->astJson(sizeplus(size))}, sizeplus(size)));
                    break;
                case MINUS:
                    op = "-";
                    children.push_back(Json("MINUS", {UnaryExp->astJson(sizeplus(size))}, sizeplus(size)));
                    break;
                case COMPLEMENT:
                    op = "~";
                    children.push_back(Json("COMPLEMENT", {UnaryExp->astJson(sizeplus(size))}, sizeplus(size)));
                    break;
            }
            return Json("Unary Expression", children, size);
        }
            // PrimaryExp form
        else {
            children.push_back(Json("Primary Expression", {PrimaryExp->astJson(sizeplus(size))}, sizeplus(size)));
            return Json("Primary Expression", children, size);
        }
    } else {   // a function call
        children.push_back(Json("Function Name", {Escape(ident)}, sizeplus(size)));
        if (!params.empty()) { // no parameters
            std::vector<std::string> paras;
            for (auto &p: params) {
                paras.push_back(p->astJson(sizeplus(size)));
            }
            children.push_back(Json("Parameters", paras, sizeplus(size)));
        }
        return Json("Function", children, size);
    }
}

IRGen::IRBase *UnaryExpAST::codegen() {
    IRGen::IRBase *res;
    auto zero_val = IRGen::Constant::get(IRGen::Type::getInt32(), 0);

    if (ident.empty()) {  // still exp, not a function call
        // UnaryOp UnaryExp form
        if (PrimaryExp == nullptr) {
            switch (OpType) {
                case PLUS:
                    // PLUS does not affect the value
                    res = UnaryExp->codegen();
                    break;
                case MINUS:
                    //res = IRGen::ConstantExpr::getSub(zero_val,IRGen::ConstantExpr::getPointerCast(UnaryExp->codegen(),IRGen::Type::getInt32PtrTy(*IR::get()->getContext())));
                    res = IR::get()->getBuilder()->CreateSub(zero_val, UnaryExp->codegen());
                    //res = IRGen::BinaryOperator::CreateNeg(UnaryExp->codegen());
                    break;
                case COMPLEMENT:
                    res = IR::get()->getBuilder()->CreateNeg(UnaryExp->codegen());
                    //res = IRGen::BinaryOperator::CreateNot(UnaryExp->codegen());
                    break;
            }
        }
            // PrimaryExp form
        else {
            res = PrimaryExp->codegen();
        }
    } else {   // a function call
        std::vector<IRGen::IRBase *> vals;
        if (params.empty()) { // no parameters
            res = IR::get()->getBuilder()->CreateCall(IR::get()->getModule()->get_function(ident));
        } else {
            for (auto &i: params)
                vals.push_back(i->codegen());
            res = IR::get()->getBuilder()->CreateCall(IR::get()->getModule()->get_function(ident), vals);
        }
    }
    return res;
}
