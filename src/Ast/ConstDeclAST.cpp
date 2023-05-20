//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstDeclAST.h"
#include "Ast/BaseAST.h"
#include <string>
#include <vector>

std::string ConstDeclAST::astJson(int size) {
    // std::unique_ptr<BaseAST> PrimitiveType;
    // std::vector<std::unique_ptr<BaseAST>> ConstDefs;
    std::vector<std::string> children;
    std::vector<std::string> defs;

    children.push_back(Json("Constant Type", {PrimitiveType->astJson(sizeplus(size))}, sizeplus(size)));
    
    for (auto &d : ConstDefs)
    {
        defs.push_back(d->astJson(sizeplus(size)));
    }
    children.push_back(Json("Constant Definitions", defs, sizeplus(size)));
    return Json("Constant Declaration", children, size);
}

// TODO: better create alloca here, since  types are unknown if more than one type is supported
llvm::Value *ConstDeclAST::codegen() {
    // auto type = dynamic_cast<PrimitiveTypeAST *>(PrimitiveType.get())->get_type();
    // auto builder = IR::get()->getBuilder();
    // TODO too sloppy
    for (auto &const_def: ConstDefs)
    {
        const_def->codegen();
        //***************************
        // auto pairs = dynamic_cast<ConstDefAST *>(const_def.get())->get_defs();
        // if (!IR::get()->isGlobeBlock())
        // {
        //     auto res = builder->CreateAlloca(type, nullptr, pairs.first);
        //     IR::get()->AddAlloca(res, pairs.first);
        //     builder->CreateStore(pairs.second, res);
        // }
        // else
        // {
        //     auto gl = new llvm::GlobalVariable(*IR::get()->getModule(),
        //                                        type,
        //                                        true,
        //                                        llvm::GlobalValue::ExternalLinkage,
        //                                        llvm::dyn_cast<llvm::Constant>(pairs.second),
        //                                        pairs.first);
        //     //IR::get()->AddGlobe(gl);
        // }
        //***************************
    }
    return nullptr;
}
