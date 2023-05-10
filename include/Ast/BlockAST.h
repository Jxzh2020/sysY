//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_BLOCKAST_H
#define SYSY_BLOCKAST_H

#include "BaseAST.h"
#include "BlockItemAST.h"

class BlockAST : public BaseAST {
public:
    std::vector<std::unique_ptr<BaseAST>> BlockItems;

    void Dump() const override;

    [[nodiscard]] bool isBranch() const;

    [[nodiscard]] llvm::Value *codegen() override;
};


#endif //SYSY_BLOCKAST_H
