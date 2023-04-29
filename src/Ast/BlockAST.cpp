//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/BlockAST.h"

void BlockAST::Dump() const {
    std::cout << "BlockAST { ";
    for (auto &i: BlockItems)
        i->Dump();
    std::cout << " }";
}

llvm::Value *BlockAST::codegen() {
    // create a basic block attached to parent function
    // moved to upper level
//    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*IR::get()->getContext(),"entry",IR::get()->getFunc());
//    IR::get()->getBuilder()->SetInsertPoint(BB);

    // empty block
    if (BlockItems.empty())
        return nullptr;
    // TODO here, it must codegen first, because if-else return is detected only when they are evaluated!
    for (auto &i: BlockItems){
        i->codegen();
        if(dynamic_cast<BlockItemAST*>(i.get())->isBranch()){
            break;
        }
    }
    return nullptr;
}

bool BlockAST::isBranch() const {
    // TODO std::any_of
    for(auto& i : BlockItems){
        auto stmt = dynamic_cast<BlockItemAST*>(i.get());
        if(stmt->isBranch() == true)
            return true;
    }
    return false;
}

