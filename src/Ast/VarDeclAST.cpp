//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/VarDeclAST.h"

void VarDeclAST::Dump() const {

}

llvm::Value *VarDeclAST::codegen() const {
    auto type = dynamic_cast<BTypeAST*>(BType.get())->get_type();
    auto& builder = IR::get()->getBuilder();
    //
    for(auto& var_def:VarDefs){
        auto pairs = dynamic_cast<VarDefAST*>(var_def.get())->get_defs();
        auto res = builder->CreateAlloca(type,nullptr,pairs.first);
        IR::get()->AddAlloca(res);
        builder->CreateStore(pairs.second, res);
    }
    return nullptr;
}
