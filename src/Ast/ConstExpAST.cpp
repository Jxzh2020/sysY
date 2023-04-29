//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstExpAST.h"

void ConstExpAST::Dump() const {

}

llvm::Value *ConstExpAST::codegen() {
    return Exp->codegen();
}
