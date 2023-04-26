//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/FunctypeAST.h"

void FunctypeAST::Dump() const {
    std::cout << "FunctypeAST { ";
    std::cout << type;
    std::cout << " }";
}

llvm::Value *FunctypeAST::codegen() const {
    return nullptr;
}

llvm::FunctionType *FunctypeAST::get_type(const std::vector<std::unique_ptr<BaseAST>>& params) const {
    // TODO FuncFParam AST push_back inside loop
    std::vector<llvm::Type*> types;
    for(auto& i: params){
        types.push_back(dynamic_cast<FuncFParamAST*>(i.get())->getType());
    }
    if(type == "int")
        return llvm::FunctionType::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),types,false);
    else
        return llvm::FunctionType::get(llvm::Type::getVoidTy(*IR::get()->getContext()),types,false);
}
