//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/InitValAST.h"

void InitValAST::Dump() const {

}

llvm::Value *InitValAST::codegen() const {
    return Exp->codegen();
}
