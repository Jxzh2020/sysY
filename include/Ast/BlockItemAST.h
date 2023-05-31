//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_BLOCKITEMAST_H
#define SYSY_BLOCKITEMAST_H

#include "BaseAST.h"
#include "StmtAST.h"

class BlockItemAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> Decl;
    std::unique_ptr<BaseAST> Stmt;

    std::string astJson(int size) override;

    [[nodiscard]] IRGen::IRBase *codegen() override;

    bool isBranch() const;
};


#endif //SYSY_BLOCKITEMAST_H
