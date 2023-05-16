//
// Created by Hao Zhong on 4/26/23.
//

#ifndef SYSY_FUNCFPARAMAST_H
#define SYSY_FUNCFPARAMAST_H

#include "BaseAST.h"
#include "PrimitiveTypeAST.h"

class FuncFParamAST : public BaseAST {
public:
    std::string ident;
    std::unique_ptr<BaseAST> PrimitiveType;

    const std::string &getName() const;

    llvm::Type *getType() const;

    std::string astJson() override {}

    [[nodiscard]] llvm::Value *codegen() override { return nullptr; }
};


#endif //SYSY_FUNCFPARAMAST_H
