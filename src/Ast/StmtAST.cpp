//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/StmtAST.h"

void StmtAST::Dump() const {
    std::cout << "StmtAST { ";
    Exp->Dump();
    std::cout << " }";
}

llvm::Value *StmtAST::codegen() {
    isEndBranch = false;

    // so far, only return inst is supported.
    auto &builder = IR::get()->getBuilder();


    //auto ret = llvm::ReturnInst::Create(*IR::get()->getContext(),llvm::ConstantInt::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),0));
    //auto ret = llvm::ReturnInst::Create(*IR::get()->getContext(),val);
    // add to the end of the current block
    //IR::get()->getBuilder()->Insert(ret);


    llvm::Value *res;
    llvm::BasicBlock *true_bb, *false_bb, *t_bb;

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
            IR::get()->SetBranch();
            isEndBranch = true;
            break;
        case IF:
            true_bb = IR::get()->NewBasicBlock();
            false_bb = IR::get()->NewBasicBlock();
            builder->CreateCondBr(bool_convert(), true_bb, false_bb);
            // has only if stmt, false_bb is the next bb.
            if (else_stmt == nullptr) {
                IR::get()->EnterBlock(true_bb);
                if_stmt->codegen();
                if(IR::get()->hasBranchAtEnd()){
                    // true_bb 已经结束了， 取消末尾branch标识
                    IR::get()->ClearBranch();
                }
                // 只有在末尾不是 break 或者 continue 的时候，才设置 true_bb 的默认跳转
                else{
                    builder->CreateBr(false_bb);
                }
                IR::get()->EnterBlock(false_bb);
            } else {
                auto next_bb = IR::get()->NewBasicBlock();
                IR::get()->EnterBlock(true_bb);
                if_stmt->codegen();
                if(IR::get()->hasBranchAtEnd()){
                    isEndBranch = true;
                    // true_bb 已经结束了， 取消末尾branch标识
                    IR::get()->ClearBranch();
                }
                    // 只有在末尾不是 break 或者 continue 的时候，才设置 true_bb 的默认跳转
                else{
                    builder->CreateBr(next_bb);
                }
                IR::get()->EnterBlock(false_bb);
                else_stmt->codegen();
                if(IR::get()->hasBranchAtEnd()){
                    // false_bb 已经结束了， 取消末尾branch标识
                    if(!isEndBranch)
                        IR::get()->ClearBranch();
                }
                    // 只有在末尾不是 break 或者 continue 的时候，才设置 false_bb 的默认跳转
                else{
                    isEndBranch = false;
                    builder->CreateBr(next_bb);
                }
                IR::get()->EnterBlock(next_bb);
            }
            res = nullptr;
            break;
        case WHILE:
            t_bb = IR::get()->NewBasicBlock();
            true_bb = IR::get()->NewBasicBlock();
            false_bb = IR::get()->NewBasicBlock();

            builder->CreateBr(t_bb);
            IR::get()->push_while(std::make_pair(t_bb,false_bb));
            // enter while clause
            IR::get()->EnterBlock(t_bb);
            // enter while or skip
            builder->CreateCondBr(bool_convert(), true_bb, false_bb);
            IR::get()->EnterBlock(true_bb);
            if_stmt->codegen();
            if(IR::get()->hasBranchAtEnd()){
                // while 已经结束了， 取消末尾branch标识
                IR::get()->ClearBranch();
            }
                // 只有在末尾不是 break 或者 continue 的时候，才设置 while 的默认跳转
            else{
                // jump back to while clause
                builder->CreateBr(t_bb);
            }
            // skip while stmt, compilation goes on
            IR::get()->pop_while();
            IR::get()->EnterBlock(false_bb);
            res = nullptr;
            break;
        case BREAK:
            res = nullptr;
            if(!IR::get()->isInWhile()){
                std::cout << "break Not in loop." << std::endl;
                exit(1);
            }
            else{
                builder->CreateBr(IR::get()->false_bb());
                IR::get()->SetBranch();
            }
            isEndBranch = true;
            break;
        case CONTINUE:
            ;
            res = nullptr;
            if(!IR::get()->isInWhile()){
                std::cout << "continue Not in loop." << std::endl;
                exit(1);
            }
            else{
                builder->CreateBr(IR::get()->condition_bb());
                IR::get()->SetBranch();
            }
            isEndBranch = true;
            break;
    }
    return res;
}

llvm::Value *StmtAST::bool_convert() const {

    auto res = Exp->codegen();
    if (res->getType() == llvm::Type::getInt1Ty(*IR::get()->getContext())) {
        return res;
    }
        // not a boolean type
    else {
        return IR::get()->getBuilder()->CreateICmpNE(res, llvm::ConstantInt::get(*IR::get()->getContext(),
                                                                                 llvm::APInt(32, 0)));
    }
}

bool StmtAST::isBranch() const {
    // codegen() evaluates if a stmt is an EndBranch
    return isEndBranch;
    bool has_branch_block = false;
    if(type == BLOCK){
        has_branch_block = dynamic_cast<BlockAST*>(Block.get())->isBranch();
    }
    return type == RET || type == BREAK || type == CONTINUE || has_branch_block;
}
