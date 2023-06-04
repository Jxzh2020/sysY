//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/BlockAST.h"

std::string BlockAST::astJson(int size) {
    // std::vector<std::unique_ptr<BaseAST>> BlockItems;
    std::vector<std::string> children;
    for (auto &b: BlockItems)
        children.push_back(b->astJson(sizeplus(size)));
    return Json("Blk", children, size);
}

IRGen::IRBase *BlockAST::codegen() {
    // create a basic block attached to parent function
    // moved to upper level
    //    IRGen::BasicBlock *BB =
    //    IRGen::BasicBlock::Create(*IR::get()->getContext(),"entry",IR::get()->getFunc());
    //    IR::get()->getBuilder()->SetInsertPoint(BB);

    // empty block
    if (BlockItems.empty())
        return nullptr;
    // TODO here, it must codegen first, because if-else return is detected only
    // when they are evaluated!
    for (auto &i: BlockItems) {
        i->codegen();
        if (dynamic_cast<BlockItemAST *>(i.get())->isBranch()) {
            break;
        }
    }
    return nullptr;
}

bool BlockAST::isBranch() const {
    // TODO std::any_of
    for (auto &i: BlockItems) {
        auto stmt = dynamic_cast<BlockItemAST *>(i.get());
        if (stmt->isBranch() == true)
            return true;
    }
    return false;
}
