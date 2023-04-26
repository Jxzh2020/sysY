//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/PrimaryExpAST.h"

void PrimaryExpAST::Dump() const {
    std::cout << "PrimaryExpAST { ";
    if( Exp != nullptr){
        std::cout << "( ";
        Exp->Dump();
        std::cout << " )";
    }
    else if( LVal == nullptr){
        Number->Dump();
    }
    else{
        LVal->Dump();
    }
    std::cout << " }";
}

llvm::Value *PrimaryExpAST::codegen() const {
    llvm::Value* res;
    // Number form
    if( Number != nullptr ){
        res = Number->codegen();
    }
    // ( Exp ) form
    else if(LVal == nullptr){
        res = Exp->codegen();
    }
    else{
        auto ret = LVal->codegen();
        auto local = llvm::dyn_cast<llvm::AllocaInst>(ret);
        // LVal is of local variable
        if(local != nullptr)
            res = IR::get()->getBuilder()->CreateLoad(local->getAllocatedType(),local);
        else{
            auto arg = llvm::dyn_cast<llvm::Argument>(ret);
            if(arg != nullptr)
                res = arg;
            else{
                auto global = llvm::dyn_cast<llvm::GlobalVariable>(ret);
                if(global != nullptr)
                    res = IR::get()->getBuilder()->CreateLoad(global->getType(),global);
                else    // should exit already in LValAST
                    res = nullptr;
            }
        }
    }
    return res;
}
