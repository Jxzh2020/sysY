//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_CONSTDECLAST_H
#define SYSY_CONSTDECLAST_H

#include "BaseAST.h"
#include "ConstDefAST.h"
#include "PrimitiveTypeAST.h"

class ConstDeclAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> PrimitiveType;
    std::vector<std::unique_ptr<BaseAST>> ConstDefs;

    std::string astJson(int size) override;

    [[nodiscard]] IRGen::IRBase *codegen() override;
};


#endif //SYSY_CONSTDECLAST_H
