//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_STMTAST_H
#define SYSY_STMTAST_H

#include "BaseAST.h"

class StmtAST: public BaseAST{
public:
    enum{
        ASSIGN,
        EXP,
        BLOCK,
        RET
    } type;
    std::unique_ptr<BaseAST> LVal;
    std::unique_ptr<BaseAST> Exp;
    std::unique_ptr<BaseAST> Block;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};


#endif //SYSY_STMTAST_H
