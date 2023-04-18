//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_BLOCKAST_H
#define SYSY_BLOCKAST_H

#include "BaseAST.h"

class BlockAST: public BaseAST{
public:
    std::unique_ptr<BaseAST> stmt;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};


#endif //SYSY_BLOCKAST_H