//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/UnaryExpAST.h"

void UnaryExpAST::Dump() const {
    std::cout << "UnaryExpAST { ";
    if( PrimaryExp == nullptr){
        std::cout << (OpType == PLUS ? "PLUS" : OpType == MINUS ? "MINUS" : "COMPLEMENT");
        UnaryExp->Dump();
    }
    else
        PrimaryExp->Dump();
    std::cout << " }";
}

llvm::Value *UnaryExpAST::codegen() const {
    return nullptr;
}
