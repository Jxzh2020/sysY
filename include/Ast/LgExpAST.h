//
// Created by Hao Zhong on 4/20/23.
//

#ifndef SYSY_LGEXPAST_H
#define SYSY_LGEXPAST_H

#include "BaseAST.h"

class LgExpAST: public BaseAST {
public:
    LogicOp type;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};


#endif //SYSY_LGEXPAST_H
