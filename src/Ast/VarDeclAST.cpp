//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/VarDeclAST.h"

void VarDeclAST::Dump() const {

}

IRGen::IRBase *VarDeclAST::codegen() {
    auto type = dynamic_cast<PrimitiveTypeAST *>(PrimitiveType.get())->get_type();
    auto &builder = IR::get()->getBuilder();
    //
    for (auto &var_def: VarDefs) {
        auto pairs = dynamic_cast<VarDefAST *>(var_def.get())->get_defs();
        if (!IR::get()->isGlobeBlock()) {
            auto res = builder->CreateAlloca(type, pairs.first);
            IR::get()->AddAlloca(res, pairs.first);
            builder->CreateStore(pairs.second, res);
        } else {
            auto gl = IRGen::GlobalVariable::Create(IR::get()->getModule().get(),
                                               type,
                                               false,
                                               pairs.second,
                                               pairs.first);
            //IR::get()->AddGlobe(gl);
        }
    }
    return nullptr;
}
