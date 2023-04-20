//
// Created by Hao Zhong on 4/20/23.
//

#ifndef SYSY_COMPEXPAST_H
#define SYSY_COMPEXPAST_H

#include "BaseAST.h"

class CompExpAST: public BaseAST {
public:
    CompOp type;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};


#endif //SYSY_COMPEXPAST_H
