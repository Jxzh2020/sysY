//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_BLOCKITEMAST_H
#define SYSY_BLOCKITEMAST_H

#include "BaseAST.h"

class BlockItemAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> Decl;
    std::unique_ptr<BaseAST> Stmt;

    void Dump() const override;

    [[nodiscard]] llvm::Value *codegen() const override;
};


#endif //SYSY_BLOCKITEMAST_H
