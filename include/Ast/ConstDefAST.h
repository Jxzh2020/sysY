//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_CONSTDEFAST_H
#define SYSY_CONSTDEFAST_H

#include "BaseAST.h"

class ConstDefAST : public BaseAST {
public:
    std::string ident;
    std::unique_ptr<BaseAST> ConstInitVal;

    void Dump() const override;

    [[nodiscard]] IRGen::IRBase *codegen() override;

    [[nodiscard]] std::pair<const std::string &, IRGen::IRBase *> get_defs() const;
};


#endif //SYSY_CONSTDEFAST_H
