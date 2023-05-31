//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_CONSTDEFAST_H
#define SYSY_CONSTDEFAST_H

#include "BaseAST.h"
#include "Ast/PrimitiveTypeAST.h"
#include <llvm-14/llvm/IR/Type.h>

class ConstDefAST : public BaseAST {
public:
    std::string ident;
    // llvm::Type* type;
    std::unique_ptr<BaseAST> ConstExp;
    std::unique_ptr<BaseAST> ConstInitVal;

    std::string astJson(int size) override;

    [[nodiscard]] IRGen::IRBase *codegen() override;

    [[nodiscard]] std::pair<const std::string &, IRGen::IRBase *> get_defs() const;
};

#endif //SYSY_CONSTDEFAST_H

// const int a = 1, b = 2;