//
// Created by Hao Zhong on 4/26/23.
//

#ifndef SYSY_FUNCFPARAMAST_H
#define SYSY_FUNCFPARAMAST_H

#include "BaseAST.h"
#include "BTypeAST.h"

class FuncFParamAST: public BaseAST {
public:
    std::string ident;
    std::unique_ptr<BaseAST> BType;

    const std::string& getName() const;
    llvm::Type* getType() const;
    void Dump() const override {}
    [[nodiscard]] llvm::Value* codegen() const override { return nullptr;}
};


#endif //SYSY_FUNCFPARAMAST_H