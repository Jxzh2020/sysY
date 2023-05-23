//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstInitValAST.h"

void ConstInitValAST::Dump() const {

}

IRGen::IRBase *ConstInitValAST::codegen() {
    return ConstExp->codegen();
}
