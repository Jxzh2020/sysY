//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_FUNCTYPEAST_H
#define SYSY_FUNCTYPEAST_H

#include "BaseAST.h"

class FunctypeAST: public BaseAST{
public:
    std::string type;
    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
    [[nodiscard]] llvm::FunctionType* get_type() const;
};


#endif //SYSY_FUNCTYPEAST_H
