//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_UNARYEXPAST_H
#define SYSY_UNARYEXPAST_H

#include "BaseAST.h"

class UnaryExpAST: public BaseAST {
public:
    std::unique_ptr<BaseAST> PrimaryExp;
    std::unique_ptr<BaseAST> UnaryExp;
    UnaryOp OpType;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};


#endif //SYSY_UNARYEXPAST_H
