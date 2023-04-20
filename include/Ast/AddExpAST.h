//
// Created by Hao Zhong on 4/20/23.
//

#ifndef SYSY_ADDEXPAST_H
#define SYSY_ADDEXPAST_H

#include "BaseAST.h"

class AddExpAST: public BaseAST {
public:
    std::unique_ptr<BaseAST> MulExp;
    std::unique_ptr<BaseAST> AddExp;
    UnaryOp type;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};


#endif //SYSY_ADDEXPAST_H
