//
// Created by Hao Zhong on 4/25/23.
//

#ifndef SYSY_INITVALAST_H
#define SYSY_INITVALAST_H

#include "BaseAST.h"

class InitValAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> Exp;

    void Dump() const override;

    [[nodiscard]] llvm::Value *codegen() const override;
};


#endif //SYSY_INITVALAST_H
