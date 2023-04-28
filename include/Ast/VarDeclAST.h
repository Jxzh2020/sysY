//
// Created by Hao Zhong on 4/25/23.
//

#ifndef SYSY_VARDECLAST_H
#define SYSY_VARDECLAST_H


#include "BaseAST.h"
#include "VarDefAST.h"
#include "PrimitiveTypeAST.h"

class VarDeclAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> PrimitiveType;
    std::vector<std::unique_ptr<BaseAST>> VarDefs;

    void Dump() const override;

    [[nodiscard]] llvm::Value *codegen() const override;
};


#endif //SYSY_VARDECLAST_H
