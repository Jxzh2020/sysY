//
// Created by Hao Zhong on 4/21/23.
//

#ifndef SYSY_BTYPEAST_H
#define SYSY_BTYPEAST_H

#include "BaseAST.h"

class BTypeAST: public BaseAST {
public:
    std::string type;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
    [[nodiscard]] llvm::Type* get_type() const;

};


#endif //SYSY_BTYPEAST_H
