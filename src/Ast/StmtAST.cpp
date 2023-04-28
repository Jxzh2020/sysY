//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/StmtAST.h"

void StmtAST::Dump() const {
    std::cout << "StmtAST { ";
    Exp->Dump();
    std::cout << " }";
}

llvm::Value *StmtAST::codegen() const {
    // so far, only return inst is supported.
    auto &builder = IR::get()->getBuilder();


    //auto ret = llvm::ReturnInst::Create(*IR::get()->getContext(),llvm::ConstantInt::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),0));
    //auto ret = llvm::ReturnInst::Create(*IR::get()->getContext(),val);
    // add to the end of the current block
    //IR::get()->getBuilder()->Insert(ret);


    llvm::Value *res;
    llvm::BasicBlock *BB, *a_BB;

    switch (type) {
        case ASSIGN:
            res = builder->CreateStore(Exp->codegen(), LVal->codegen());   // assignment operation
            break;
        case EXP:
            if (Exp == nullptr) {
                res = nullptr;
                break;
            } else
                res = Exp->codegen();
            break;
        case BLOCK:
            IR::get()->NewLogicalBlockStart();
            res = Block->codegen();
            // after the inner block is down, return to outer branch logically;
            IR::get()->NewLogicalBlockEnd();
            break;
        case RET:
            // return ; void
            if (Exp == nullptr) {
                res = IR::get()->getBuilder()->CreateRetVoid();
            } else {
//                    if(auto I = llvm::dyn_cast<llvm::AllocaInst>(val))
//                        res = IR::get()->getBuilder()->CreateLoad(I->getAllocatedType(),val);
//                    else
                res = Exp->codegen();
                res = builder->CreateRet(res);
            }
            break;
        case IF:
            auto true_bb = IR::get()->NewBasicBlock();
            auto false_bb = IR::get()->NewBasicBlock();
            auto b_ret = bool_convert();
            builder->CreateCondBr(b_ret,true_bb,false_bb);
            // has only if stmt, false_bb is the next bb.
            if(else_stmt == nullptr){
                IR::get()->EnterBlock(true_bb);
                if_stmt->codegen();
                builder->CreateBr(false_bb);
                IR::get()->EnterBlock(false_bb);
            }
            else{
                auto next_bb = IR::get()->NewBasicBlock();
                IR::get()->EnterBlock(true_bb);
                if_stmt->codegen();
                builder->CreateBr(next_bb);
                IR::get()->EnterBlock(false_bb);
                else_stmt->codegen();
                builder->CreateBr(next_bb);
                IR::get()->EnterBlock(next_bb);
            }
            res = nullptr;
    }
    return res;
}

llvm::Value *StmtAST::bool_convert() const {
    auto res = Exp->codegen();
    if(res->getType() == llvm::Type::getInt1Ty(*IR::get()->getContext())){
        return res;
    }
    // not a boolean type
    else{
        return IR::get()->getBuilder()->CreateICmpNE(res,llvm::ConstantInt::get(*IR::get()->getContext(),llvm::APInt(32,0)));
    }
}
