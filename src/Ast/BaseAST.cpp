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
    TheModule = std::make_unique<llvm::Module>("Jxzh2020_sysY", *TheContext);

    // Create a new builder for the module.
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    curFunc = nullptr;
}
