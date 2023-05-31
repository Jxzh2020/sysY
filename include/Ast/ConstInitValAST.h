//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_CONSTINITVALAST_H
#define SYSY_CONSTINITVALAST_H

#include "BaseAST.h"
#include <memory>
#include <vector>

class ConstInitValAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> ConstExp;
    std::vector<std::unique_ptr<BaseAST>> vals;

    std::string astJson(int size) override;

    [[nodiscard]] IRGen::IRBase *codegen() override;
};


#endif //SYSY_CONSTINITVALAST_H
