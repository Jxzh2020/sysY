//
// Created by Hao Zhong on 4/25/23.
//

#ifndef SYSY_INITVALAST_H
#define SYSY_INITVALAST_H

#include "BaseAST.h"

class InitValAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> Exp;

    std::string astJson(int size) override;

    [[nodiscard]] IRGen::IRBase *codegen() override;
};


#endif //SYSY_INITVALAST_H
