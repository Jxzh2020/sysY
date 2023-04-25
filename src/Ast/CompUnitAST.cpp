//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/CompUnitAST.h"

void CompUnitAST::Dump() const {
    std::cout << "CompUnitAST { ";
    for(auto &func_def:func_defs)
        func_def->Dump();
    std::cout << " }" << std::endl;
    this->codegen();
}
llvm::Value* CompUnitAST::codegen() const{
    for(auto& func_def:func_defs){
        // TODO
        func_def->codegen();
    }
//    llvm::raw_ostream &OS = llvm::outs();
//    IR::get()->getModule()->print(OS,nullptr);
//
//    bool Err = llvm::verifyModule(*IR::get()->getModule(), &llvm::errs());
    ;
    return nullptr;
}