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

llvm::Value *PrimaryExpAST::codegen() {
    llvm::Value *res;
    // Number form
    if (Number != nullptr) {
        res = Number->codegen();
    }
        // ( Exp ) form
    else if (LVal == nullptr) {
        res = Exp->codegen();
    } else {
        auto ret = LVal->codegen();
        auto local = llvm::dyn_cast<llvm::AllocaInst>(ret);
        // LVal is of local variable
        if (local != nullptr)
            res = IR::get()->getBuilder()->CreateLoad(local->getAllocatedType(), local);
        else {
            auto arg = llvm::dyn_cast<llvm::Argument>(ret);
            if (arg != nullptr)
                res = arg;
            else {
                auto global = llvm::dyn_cast<llvm::GlobalVariable>(ret);
                if (global != nullptr)
                    res = IR::get()->getBuilder()->CreateLoad(global->getType(), global);
                else    // should exit already in LValAST
                    res = nullptr;
            }
        }
    }
    return res;
}
