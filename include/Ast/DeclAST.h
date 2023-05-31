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

    std::string astJson(int size) override;

    [[nodiscard]] IRGen::IRBase *codegen() override;
};


#endif //SYSY_DECLAST_H
