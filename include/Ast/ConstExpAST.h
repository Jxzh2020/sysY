//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_CONSTEXPAST_H
#define SYSY_CONSTEXPAST_H

#include "BaseAST.h"

class ConstExpAST : public BaseAST {
public:

    std::unique_ptr<BaseAST> Exp;

    std::string astJson() override;

    [[nodiscard]] llvm::Value *codegen() override;
};


#endif //SYSY_CONSTEXPAST_H
