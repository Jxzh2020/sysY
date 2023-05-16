//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstExpAST.h"

std::string ConstExpAST::astJson() {

}

llvm::Value *ConstExpAST::codegen() {
    return Exp->codegen();
}
