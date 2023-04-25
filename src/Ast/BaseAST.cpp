//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/BaseAST.h"

std::unique_ptr<IR> IR::instance(new IR);

std::unique_ptr<IR>& IR::get() {
    return instance;
}

IR::IR() {
    // Open a new context and module.
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("jxzh_sysY", *TheContext);

    // Create a new builder for the module.
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    curFunc = nullptr;
    curblock = nullptr;
    uid = 0;
}

//void IR::RetParent() {
//    // release all the alloca of current logical block;
//    for(auto i: alloca[logical_block_of[curblock]]){
//        Builder->CreateLifetimeEnd(i);
//    }
//
//    // branch to the end of the parent logical block
//    // Create the BB of the parent logical block
//    auto block = llvm::BasicBlock::Create(TheContext,this->CreateName(),curFunc);
//    basic_blocks_of[parent_block_of_logical[logical_block_of[curblock]]].push_back(block);
//    logical_block_of[block] = parent_block_of_logical[logical_block_of[curblock]];
//
//    // jump back to parent block
//    Builder->CreateBr(block);
//    Builder->SetInsertPoint(block);
//    // switch to parent block
//    curblock = block;
//}

void IR::NewLogicalBlockStart() {
    auto l_b = llvm::BasicBlock::Create(*TheContext,this->CreateName(),curFunc);
    basic_blocks_of[l_b].push_back(l_b);

    // if is the definition block
    if(curblock == nullptr)
        parent_block_of_logical[l_b] = nullptr;
    // a nested block
    else{
        parent_block_of_logical[l_b] = logical_block_of[curblock];
        Builder->CreateBr(l_b);
    }
    Builder->SetInsertPoint(l_b);
    curblock = l_b;
}

void IR::NewLogicalBlockEnd() {
    // if is the definition block, no need to return to parent, and a RET inst is done manually.
    if(parent_block_of_logical[logical_block_of[curblock]] == nullptr)
        return ;
    for(auto i: alloca[logical_block_of[curblock]]){
        Builder->CreateLifetimeEnd(i);
    }

    auto block = llvm::BasicBlock::Create(*TheContext,this->CreateName(),curFunc);
    basic_blocks_of[parent_block_of_logical[logical_block_of[curblock]]].push_back(block);
    logical_block_of[block] = parent_block_of_logical[logical_block_of[curblock]];

    // jump back to parent block
    Builder->CreateBr(block);
    Builder->SetInsertPoint(block);
    // switch to parent block
    curblock = block;
}

llvm::BasicBlock *IR::NewBasicBlock() {
    auto bb = llvm::BasicBlock::Create(*TheContext,CreateName(),curFunc);
    basic_blocks_of[logical_block_of[curblock]].push_back(bb);
    logical_block_of[bb] = logical_block_of[curblock];
    return bb;
}

void IR::AddAlloca(llvm::AllocaInst * al) {
    alloca[logical_block_of[curblock]].push_back(al);
    // top block
    if(parent_block_of_logical[logical_block_of[curblock]] == nullptr)
        return ;
    Builder->CreateLifetimeStart(al);
}
