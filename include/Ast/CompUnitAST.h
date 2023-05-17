//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_COMPUNITAST_H
#define SYSY_COMPUNITAST_H

#include "BaseAST.h"

class CompUnitAST : public BaseAST {
public:
    // 用智能指针管理对象
    std::vector<std::unique_ptr<BaseAST>> globe;

    std::string astJson(int size) override;

    [[nodiscard]] llvm::Value *codegen() override;
};


#endif //SYSY_COMPUNITAST_H
