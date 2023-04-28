//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstDeclAST.h"

void ConstDeclAST::Dump() const {

}

// TODO: better create alloca here, since  types are unknown if more than one type is supported
llvm::Value *ConstDeclAST::codegen() const {
    auto type = dynamic_cast<PrimitiveTypeAST *>(PrimitiveType.get())->get_type();
    auto &builder = IR::get()->getBuilder();
    // TODO too sloppy
    for (auto &const_def: ConstDefs) {
        auto pairs = dynamic_cast<ConstDefAST *>(const_def.get())->get_defs();
        if (!IR::get()->isGlobeBlock()) {
            //builder->CreateAlloca(type,pairs.second,pairs.first);
            auto res = builder->CreateAlloca(type, nullptr, pairs.first);
            IR::get()->AddAlloca(res, pairs.first);
            builder->CreateStore(pairs.second, res);
        } else {
            auto gl = new llvm::GlobalVariable(*IR::get()->getModule(),
                                               type,
                                               true,
                                               llvm::GlobalValue::ExternalLinkage,
                                               llvm::dyn_cast<llvm::Constant>(pairs.second),
                                               pairs.first);
            //IR::get()->AddGlobe(gl);
        }
    }
    return nullptr;
}
