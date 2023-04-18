//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_FUNCDEFAST_H
#define SYSY_FUNCDEFAST_H

#include "BaseAST.h"

class FuncDefAST : public BaseAST{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};


#endif //SYSY_FUNCDEFAST_H
