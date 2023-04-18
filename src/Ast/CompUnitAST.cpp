//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/CompUnitAST.h"

void CompUnitAST::Dump() const {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }" << std::endl;
    this->codegen();
}
llvm::Value* CompUnitAST::codegen() const{
    this->func_def->codegen();
    llvm::raw_ostream &OS = llvm::outs();
    IR::get()->getModule()->print(OS,nullptr);
    return nullptr;
}