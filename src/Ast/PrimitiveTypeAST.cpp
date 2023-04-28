//
// Created by Hao Zhong on 4/28/23.
//

#include "Ast/PrimitiveTypeAST.h"

llvm::Type *PrimitiveTypeAST::get_type() const {
    if (type == "int")
        return llvm::Type::getInt32Ty(*IR::get()->getContext());
    else if (type == "void")
        return llvm::Type::getVoidTy(*IR::get()->getContext());
    else
        return nullptr;
}

llvm::FunctionType *PrimitiveTypeAST::get_type(const std::vector <std::unique_ptr<BaseAST>> &params) const {
    // TODO FuncFParam AST push_back inside loop, fixed
    std::vector<llvm::Type *> types;
    types.reserve(params.size());
    for (auto &i: params) {
        types.emplace_back(dynamic_cast<FuncFParamAST *>(i.get())->getType());
    }
    if (type == "int")
        return llvm::FunctionType::get(llvm::Type::getInt32Ty(*IR::get()->getContext()), types, false);
    else if (type == "void")
        return llvm::FunctionType::get(llvm::Type::getVoidTy(*IR::get()->getContext()), types, false);
    else
        return nullptr;
}
