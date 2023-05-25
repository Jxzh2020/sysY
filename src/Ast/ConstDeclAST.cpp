//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstDeclAST.h"

void ConstDeclAST::Dump() const {

}

// TODO: better create alloca here, since  types are unknown if more than one type is supported
IRGen::IRBase *ConstDeclAST::codegen() {
    auto type = dynamic_cast<PrimitiveTypeAST *>(PrimitiveType.get())->get_type();
    auto &builder = IR::get()->getBuilder();
    // TODO too sloppy
    for (auto &const_def: ConstDefs) {
        auto pairs = dynamic_cast<ConstDefAST *>(const_def.get())->get_defs();
        if (!IR::get()->isGlobeBlock()) {
            //builder->CreateAlloca(type,pairs.second,pairs.first);
            auto res = builder->CreateAlloca(type, pairs.first, true);
            IR::get()->AddAlloca(res, pairs.first);
            builder->CreateStore(pairs.second, res);
        } else {
            auto gl = IRGen::GlobalVariable::Create(IR::get()->getModule().get(),
                                               type,
                                               true,
                                               pairs.second,
                                               pairs.first);
            //IR::get()->AddGlobe(gl);
        }
    }
    return nullptr;
}
