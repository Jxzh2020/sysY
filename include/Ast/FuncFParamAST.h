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

    IRGen::Type *getType() const;

    void Dump() const override {}

    [[nodiscard]] IRGen::IRBase *codegen() override { return nullptr; }
};


#endif //SYSY_FUNCFPARAMAST_H
