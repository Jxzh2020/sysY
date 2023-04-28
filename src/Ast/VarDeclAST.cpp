//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/VarDeclAST.h"

void VarDeclAST::Dump() const {

}

llvm::Value *VarDeclAST::codegen() const {
    auto type = dynamic_cast<PrimitiveTypeAST *>(PrimitiveType.get())->get_type();
    auto &builder = IR::get()->getBuilder();
    //
    for (auto &var_def: VarDefs) {
        auto pairs = dynamic_cast<VarDefAST *>(var_def.get())->get_defs();
        if (!IR::get()->isGlobeBlock()) {
            auto res = builder->CreateAlloca(type, nullptr, pairs.first);
            IR::get()->AddAlloca(res, pairs.first);
            builder->CreateStore(pairs.second, res);
        } else {
            auto gl = new llvm::GlobalVariable(*IR::get()->getModule(),
                                               type,
                                               false,
                                               llvm::GlobalValue::ExternalLinkage,
                                               llvm::dyn_cast<llvm::Constant>(pairs.second),
                                               pairs.first);
            //IR::get()->AddGlobe(gl);
        }
    }
    return nullptr;
}
