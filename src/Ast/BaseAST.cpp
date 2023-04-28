//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/BaseAST.h"

std::unique_ptr<IR> IR::instance(new IR);

std::unique_ptr<IR> &IR::get() {
    return instance;
}

IR::IR() {
    // Open a new context and module.
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("jxzh_sysY", *TheContext);

    // Create a new builder for the module.
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    curFunc = nullptr;
    Func_Context = nullptr;
    isglobe = true;
    hasBranch = false;
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
    auto l_b = llvm::BasicBlock::Create(*TheContext, this->CreateName(), curFunc);
    // TODO wired usage
    Func_Context->basic_blocks_of[l_b].push_back(l_b);
    Func_Context->logical_block_of[l_b] = l_b;

    // if is the definition block
    if (Func_Context->curblock == nullptr)
        Func_Context->parent_block_of_logical[l_b] = nullptr;
        // a nested block
    else {
        Func_Context->parent_block_of_logical[l_b] = Func_Context->logical_block_of[Func_Context->curblock];
        Builder->CreateBr(l_b);
    }
    Builder->SetInsertPoint(l_b);
    Func_Context->curblock = l_b;
}

void IR::NewLogicalBlockEnd() {
    // if at the end of block is a branch, do nothing
    if(hasBranch)
        return ;
    // if is the definition block, no need to return to parent, and a RET inst is done manually.
    if (Func_Context->parent_block_of_logical[Func_Context->logical_block_of[Func_Context->curblock]] == nullptr)
        return;
    for (auto i: Func_Context->alloca[Func_Context->logical_block_of[Func_Context->curblock]]) {
        Builder->CreateLifetimeEnd(i);
    }

    auto block = llvm::BasicBlock::Create(*TheContext, this->CreateName(), curFunc);
    Func_Context->basic_blocks_of[Func_Context->parent_block_of_logical[Func_Context->logical_block_of[Func_Context->curblock]]].push_back(
            block);
    Func_Context->logical_block_of[block] = Func_Context->parent_block_of_logical[Func_Context->logical_block_of[Func_Context->curblock]];

    // jump back to parent block
    Builder->CreateBr(block);
    Builder->SetInsertPoint(block);
    // switch to parent block
    Func_Context->curblock = block;
}

llvm::BasicBlock *IR::NewBasicBlock() {
    auto bb = llvm::BasicBlock::Create(*TheContext, CreateName(), curFunc);
    Func_Context->basic_blocks_of[Func_Context->logical_block_of[Func_Context->curblock]].push_back(bb);
    Func_Context->logical_block_of[bb] = Func_Context->logical_block_of[Func_Context->curblock];
    return bb;
}

void IR::AddAlloca(llvm::AllocaInst *al, const std::string &o_name) {
    /****    Add to Alloca list   ****/
    Func_Context->alloca[Func_Context->logical_block_of[Func_Context->curblock]].push_back(al);

    /****    Construct Name mapping to support shadowing    ****/
    if (Func_Context->name_map.find(Func_Context->logical_block_of[Func_Context->curblock]) ==
        Func_Context->name_map.end()) {
        Func_Context->name_map[Func_Context->logical_block_of[Func_Context->curblock]] = std::make_unique<std::unordered_map<std::string, llvm::AllocaInst *>>();
    }
    auto &map = Func_Context->name_map[Func_Context->logical_block_of[Func_Context->curblock]];
    if (map->find(o_name) != map->end()) {
        std::cout << "Error! Repeated variable name in one block!" << std::endl;
        exit(1);
    }
    (*map)[o_name] = al;


    /****    Set Variable LifeTime    ****/
    // top block
    if (Func_Context->parent_block_of_logical[Func_Context->logical_block_of[Func_Context->curblock]] == nullptr)
        return;
    Builder->CreateLifetimeStart(al);
}

llvm::Value *IR::GetAlloca(const std::string &name) {
    return GetAlloca(name, Func_Context->curblock);
}

llvm::Value *IR::GetAlloca(const std::string &name, llvm::BasicBlock *cur) {
    if (cur == nullptr) {
        return nullptr;
    }
    if (Func_Context->name_map.find(Func_Context->logical_block_of[cur]) == Func_Context->name_map.end()) {
        return GetAlloca(name, Func_Context->parent_block_of_logical[Func_Context->logical_block_of[cur]]);
    }
    auto &map = Func_Context->name_map[Func_Context->logical_block_of[cur]];
    if (map.get()->find(name) == map->end()) {
        return GetAlloca(name, Func_Context->parent_block_of_logical[Func_Context->logical_block_of[cur]]);
    } else
        return (*map)[name];
}
