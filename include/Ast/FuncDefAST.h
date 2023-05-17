//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_FUNCDEFAST_H
#define SYSY_FUNCDEFAST_H

#include "BaseAST.h"
#include "PrimitiveTypeAST.h"
#include "FuncFParamAST.h"

class FuncDefAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;
    std::vector<std::unique_ptr<BaseAST>> params;

    std::string astJson(int size) override;

    [[nodiscard]] llvm::Value *codegen() override;

    void SetName(llvm::Function *) const;
};


#endif //SYSY_FUNCDEFAST_H
