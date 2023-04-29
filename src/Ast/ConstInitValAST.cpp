//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstInitValAST.h"

void ConstInitValAST::Dump() const {

}

llvm::Value *ConstInitValAST::codegen() {
    return ConstExp->codegen();
}
