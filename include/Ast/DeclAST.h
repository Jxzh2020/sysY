//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_DECLAST_H
#define SYSY_DECLAST_H

#include "BaseAST.h"

class DeclAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> ConstDecl;
    std::unique_ptr<BaseAST> VarDecl;

    void Dump() const override;

    [[nodiscard]] llvm::Value *codegen() override;
};


#endif //SYSY_DECLAST_H
