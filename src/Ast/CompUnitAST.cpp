//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/CompUnitAST.h"

void CompUnitAST::Dump() const {
    std::cout << "CompUnitAST { ";
    for (auto &func_def: this->globe)
        func_def->Dump();
    std::cout << " }" << std::endl;
}

IRGen::IRBase *CompUnitAST::codegen() {
    for (auto &item: this->globe) {
        //
        // TODO
        item->codegen();
    }
//    IRGen::raw_ostream &OS = IRGen::outs();
//    IR::get()->getModule()->print(OS,nullptr);
//
//    bool Err = IRGen::verifyModule(*IR::get()->getModule(), &IRGen::errs());
    ;
    return nullptr;
}