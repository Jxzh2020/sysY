//
// Created by Hao Zhong on 4/25/23.
//

#ifndef SYSY_VARDEFAST_H
#define SYSY_VARDEFAST_H

#include "BaseAST.h"

class VarDefAST: public BaseAST {
public:
    std::string ident;
    std::unique_ptr<BaseAST> InitVal;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
    std::pair<const std::string &, llvm::Value *> get_defs() const;

};


#endif //SYSY_VARDEFAST_H
