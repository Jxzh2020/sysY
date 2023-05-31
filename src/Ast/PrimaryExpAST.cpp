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
    
    if (Number != nullptr)// Number form
    {
        res = Number->codegen();
    }
    else if (LVal == nullptr)// ( Exp ) form
    {
        res = Exp->codegen();
    }
    else//LVal form
    {
        auto res = LVal->codegen();

        auto local = llvm::dyn_cast<llvm::AllocaInst>(res);

        if (local != nullptr)// LVal is of local variable
        {
            res = IR::get()->getBuilder()->CreateLoad(local->getAllocatedType(), local);
        }
        else// LVal is of global variable
        {
            auto arg = llvm::dyn_cast<llvm::Argument>(res);
            if (arg != nullptr)//function argument
            {
                res = arg;
            }// global variable
            else
            {
                auto global = llvm::dyn_cast<llvm::GlobalVariable>(res);
                if (global != nullptr)
                    res = IR::get()->getBuilder()->CreateLoad(global->getType(), global);
                else    // should exit already in LValAST, now modified to array CreateGEP
                    res = IR::get()->getBuilder()->CreateLoad(, res);
            }
        }
    }
    return res;
}
