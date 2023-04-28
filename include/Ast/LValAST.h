//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_LVALAST_H
#define SYSY_LVALAST_H

#include "BaseAST.h"

class LValAST : public BaseAST {
public:
    std::string ident;

    void Dump() const override;

    [[nodiscard]] llvm::Value *codegen() const override;
};


#endif //SYSY_LVALAST_H
