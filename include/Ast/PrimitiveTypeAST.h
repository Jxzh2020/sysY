//
// Created by Hao Zhong on 4/28/23.
//

#ifndef SYSY_PRIMITIVETYPEAST_H
#define SYSY_PRIMITIVETYPEAST_H

#include "BaseAST.h"
#include "FuncFParamAST.h"

class PrimitiveTypeAST: public BaseAST {
public:
    std::string type;

    [[nodiscard]] llvm::Type* get_type() const;
    [[nodiscard]] llvm::FunctionType* get_type(const std::vector<std::unique_ptr<BaseAST>>& ) const;

    void Dump() const override {}
    [[nodiscard]] llvm::Value* codegen() const override { return nullptr;}

};


#endif //SYSY_PRIMITIVETYPEAST_H
