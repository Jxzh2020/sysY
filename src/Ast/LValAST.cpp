//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/LValAST.h"
#include "Ast/BaseAST.h"
#include <llvm-14/llvm/IR/Instructions.h>
#include <llvm-14/llvm/IR/Value.h>
#include <llvm-14/llvm/Support/Casting.h>
#include <string>
#include <vector>

std::string LValAST::astJson(int size) {
    // std::string ident;
    // std::unique_ptr<BaseAST> Exp;
    return Json("Load Value", {Escape(ident)}, size);
}

llvm::Value *LValAST::codegen() {
    // auto ref = IR::get()->GetAlloca(ident);
    auto &builder = IR::get()->getBuilder();
    if(Exp==nullptr)//variable
    {
        // TODO: unsolved constant assignment

        // 1. local variable, it seems that LLVM automatically shadows variables in nested logical blocks
        // auto ref = IR::get()->getFunc()->getValueSymbolTable()->lookup(ident);
        auto ref = IR::get()->GetAlloca(ident);
        // 2. function parameter
        if (ref == nullptr && !IR::get()->getFunc()->arg_empty()) {
            auto argIter = IR::get()->getFunc()->arg_begin();
            auto argEnd = IR::get()->getFunc()->arg_end();
            for (; argIter != argEnd; argIter++)
                if (argIter->getName().str() == ident) {
                    ref = argIter;  //IR::get()->getBuilder()->CreateLoad(argIter->getType(),argIter);
                    break;
                }
        }
        // 3. global variable
        if (ref == nullptr) {
            ref = IR::get()->getModule()->getGlobalVariable(ident);
        }
        // not found
        if (ref == nullptr) {
            std::cout << "Error! LVal not found!" << std::endl;
            exit(1);
        }
        //auto val = IR::get()->getBuilder()->CreateLoad(static_cast<llvm::AllocaInst*>(ref)->getAllocatedType(),ref);
        //llvm::AllocaInst* I = llvm::dyn_cast<llvm::AllocaInst>(val)
        //if(auto val = llvm::dyn_cast<llvm::AllocaInst>(ref))
        //    return IR::get()->getBuilder()->CreateLoad(val->getAllocatedType(),val);
        return ref;
    }
    else//array
    {
        llvm::LLVMContext* c = IR::get()->getContext().get();
        llvm::LLVMContext& context = *c;
        auto arr = llvm::dyn_cast<llvm::AllocaInst>(IR::get()->GetAlloca(ident));
        if(arr)//local array
        {
            auto index = Exp->codegen();
            llvm::ConstantInt* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            llvm::Value* indices[] = { zero, index };
            llvm::Value* elemPtr = builder->CreateGEP(arr->getAllocatedType(),arr, indices);
            // Create a constant for the initial value of the j-th element
            return elemPtr;
        }

        // 2. function parameter
        if (arr == nullptr && !IR::get()->getFunc()->arg_empty()) {
            ;
        }
        // 3. global variable
        if (arr == nullptr) {
            auto arr = llvm::dyn_cast<llvm::AllocaInst>(IR::get()->getModule()->getGlobalVariable(ident));
            auto index = Exp->codegen();
            llvm::ConstantInt* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            llvm::Value* indices[] = { zero, index };
            llvm::Value* elemPtr = builder->CreateGEP(arr->getAllocatedType(),arr, indices);
            // Create a constant for the initial value of the j-th element
            assert(llvm::dyn_cast<llvm::AllocaInst>(elemPtr) == nullptr);
            return elemPtr;
        }
        // not found
        if (arr == nullptr) {
            std::cout << "Error! LVal not found!" << std::endl;
            exit(1);
        }
        //auto val = IR::get()->getBuilder()->CreateLoad(static_cast<llvm::AllocaInst*>(ref)->getAllocatedType(),ref);
        //llvm::AllocaInst* I = llvm::dyn_cast<llvm::AllocaInst>(val)
        //if(auto val = llvm::dyn_cast<llvm::AllocaInst>(ref))
        //    return IR::get()->getBuilder()->CreateLoad(val->getAllocatedType(),val);
        return arr;

        // llvm::Value* value = builder->CreateLoad(ptr);
        
    }
}
