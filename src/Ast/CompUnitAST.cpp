//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/CompUnitAST.h"

std::string CompUnitAST::astJson(int size) {
    // std::cout << "CompUnitAST { ";
    // for (auto &func_def: this->globe)
    //     func_def->astJson();
    // std::cout << " }" << std::endl;
    std::vector<std::string> globes;
    for (auto &g: globe)
    {
        globes.push_back(g->astJson(sizeplus(size)));
    }
    return Json("CompUnit", globes, size);
}

llvm::Value *CompUnitAST::codegen() {
    for (auto &item: this->globe) {
        //
        // TODO
        item->codegen();
    }
//    llvm::raw_ostream &OS = llvm::outs();
//    IR::get()->getModule()->print(OS,nullptr);
//
//    bool Err = llvm::verifyModule(*IR::get()->getModule(), &llvm::errs());
    ;
    return nullptr;
}