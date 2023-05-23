//
// Created by Hao Zhong on 4/20/23.
//

#ifndef SYSY_LGEXPAST_H
#define SYSY_LGEXPAST_H

#include "BaseAST.h"

class LgExpAST : public BaseAST {
public:
    LogicOp type;
    // if RHS == nullptr, it's not an exp of this level!
    std::unique_ptr<BaseAST> LHS;
    std::unique_ptr<BaseAST> RHS;

    void Dump() const override;

    [[nodiscard]] IRGen::IRBase *codegen() override;
};


#endif //SYSY_LGEXPAST_H
