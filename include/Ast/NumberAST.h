//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_NUMBERAST_H
#define SYSY_NUMBERAST_H

#include "BaseAST.h"

class NumberAST : public BaseAST {
public:
    int value;

    std::string astJson(int size) override;

    [[nodiscard]] IRGen::IRBase *codegen() override;
};


#endif //SYSY_NUMBERAST_H
