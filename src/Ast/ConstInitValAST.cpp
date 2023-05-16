//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstInitValAST.h"

std::string ConstInitValAST::astJson() {

}

llvm::Value *ConstInitValAST::codegen() {
    return ConstExp->codegen();
}
