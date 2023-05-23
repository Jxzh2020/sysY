//
// Created by Hao Zhong on 4/20/23.
//

#ifndef SYSY_COMPEXPAST_H
#define SYSY_COMPEXPAST_H

#include "BaseAST.h"

class CompExpAST : public BaseAST {
public:
    CompOp type;
    // if RHS == nullptr, it's not an exp of this level!
    std::unique_ptr<BaseAST> LHS;
    std::unique_ptr<BaseAST> RHS;

    void Dump() const override;

    [[nodiscard]] IRGen::IRBase *codegen() override;
};


#endif //SYSY_COMPEXPAST_H
