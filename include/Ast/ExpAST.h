//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_EXPAST_H
#define SYSY_EXPAST_H

#include "BaseAST.h"

class ExpAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> LgExp;

    std::string astJson(int size) override;

    [[nodiscard]] llvm::Value *codegen() override;
};


#endif //SYSY_EXPAST_H
