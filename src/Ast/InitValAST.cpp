//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/InitValAST.h"

void InitValAST::Dump() const {

}

IRGen::IRBase *InitValAST::codegen() {
    return Exp->codegen();
}
