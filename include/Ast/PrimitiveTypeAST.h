//
// Created by Hao Zhong on 4/28/23.
//

#ifndef SYSY_PRIMITIVETYPEAST_H
#define SYSY_PRIMITIVETYPEAST_H

#include "BaseAST.h"
#include "FuncFParamAST.h"

class PrimitiveTypeAST : public BaseAST {
public:
    std::string type;

    [[nodiscard]] IRGen::Type *get_type() const;

    [[nodiscard]] IRGen::FunctionType *get_type(const std::vector<std::unique_ptr<BaseAST>> &) const;

    std::string astJson(int size) override;

    [[nodiscard]] IRGen::IRBase *codegen() override {
        assert(0 && "PrimitiveTypeAST::codegen() Not implemented");
        return nullptr;
    }

};


#endif //SYSY_PRIMITIVETYPEAST_H
