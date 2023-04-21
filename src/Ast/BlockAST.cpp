//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/BlockAST.h"

void BlockAST::Dump() const {
    std::cout << "BlockAST { ";
    for(auto& i:BlockItems)
        i->Dump();
    std::cout << " }";
}

llvm::Value *BlockAST::codegen() const {
    // create a basic block attached to parent function
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*IR::get()->getContext(),"entry",IR::get()->GetFunc());
    IR::get()->getBuilder()->SetInsertPoint(BB);

    for(auto& i:BlockItems)
        i->codegen();
    return BB;
}
