//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_CONSTINITVALAST_H
#define SYSY_CONSTINITVALAST_H

#include "BaseAST.h"

class ConstInitValAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> ConstExp;

    std::string astJson(int size) override;

    [[nodiscard]] llvm::Value *codegen() override;
};


#endif //SYSY_CONSTINITVALAST_H
