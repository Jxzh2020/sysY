//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_PRIMARYEXPAST_H
#define SYSY_PRIMARYEXPAST_H

#include "BaseAST.h"

class PrimaryExpAST: public BaseAST {
public:
    std::unique_ptr<BaseAST> Exp;
    std::unique_ptr<BaseAST> LVal;
    std::unique_ptr<BaseAST> Number;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};


#endif //SYSY_PRIMARYEXPAST_H
