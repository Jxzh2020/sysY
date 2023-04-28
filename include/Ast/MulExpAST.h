//
// Created by Hao Zhong on 4/20/23.
//

#ifndef SYSY_MULEXPAST_H
#define SYSY_MULEXPAST_H

#include "BaseAST.h"

class MulExpAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> UnaryExp;
    std::unique_ptr<BaseAST> MulExp;
    BinaryOp type;

    void Dump() const override;

    [[nodiscard]] llvm::Value *codegen() const override;
};


#endif //SYSY_MULEXPAST_H
