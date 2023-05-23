//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstExpAST.h"

void ConstExpAST::Dump() const {

}

IRGen::IRBase *ConstExpAST::codegen() {
    return Exp->codegen();
}
