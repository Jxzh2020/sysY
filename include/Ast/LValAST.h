//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_LVALAST_H
#define SYSY_LVALAST_H

#include "BaseAST.h"

class LValAST : public BaseAST {
public:
    std::string ident;
    std::unique_ptr<BaseAST> Exp;

    std::string astJson(int size) override;

    [[nodiscard]] IRGen::IRBase *codegen() override;
};


#endif //SYSY_LVALAST_H
