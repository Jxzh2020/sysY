//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_STMTAST_H
#define SYSY_STMTAST_H

#include "BaseAST.h"

class StmtAST : public BaseAST {
public:
    enum {
        ASSIGN,
        EXP,
        BLOCK,
        RET,
        IF,
        WHILE,
        BREAK,
        CONTINUE
    } type;
    bool matched;
    std::unique_ptr<BaseAST> LVal;
    std::unique_ptr<BaseAST> Exp;
    std::unique_ptr<BaseAST> Block;
    std::unique_ptr<BaseAST> if_stmt;   // or while_stmt in WHILE case
    std::unique_ptr<BaseAST> else_stmt;

    void Dump() const override;

    [[nodiscard]] llvm::Value *codegen() const override;

    llvm::Value *bool_convert() const;
};


#endif //SYSY_STMTAST_H
