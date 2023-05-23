//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_EXPAST_H
#define SYSY_EXPAST_H

#include "BaseAST.h"

class ExpAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> LgExp;

    void Dump() const override;

    [[nodiscard]] IRGen::IRBase *codegen() override;
};


#endif //SYSY_EXPAST_H
