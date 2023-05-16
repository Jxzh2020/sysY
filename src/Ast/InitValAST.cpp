//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/InitValAST.h"

std::string InitValAST::astJson() {

}

llvm::Value *InitValAST::codegen() {
    return Exp->codegen();
}
