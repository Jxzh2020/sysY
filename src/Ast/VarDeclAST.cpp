//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/VarDeclAST.h"

std::string VarDeclAST::astJson(int size) {
  // std::unique_ptr<BaseAST> PrimitiveType;
  // std::vector<std::unique_ptr<BaseAST>> VarDefs;
  std::vector<std::string> children;
  std::vector<std::string> defs;

  children.push_back(Json("Variable Type",
                          {PrimitiveType->astJson(sizeplus(size))},
                          sizeplus(size)));

  for (auto &v : VarDefs) {
    defs.push_back(v->astJson(sizeplus(size)));
  }
  children.push_back(Json("Variable Definitions", defs, sizeplus(size)));
  return Json("Variable Declaration", children, size);
}

IRGen::IRBase *VarDeclAST::codegen() {
  auto type = dynamic_cast<PrimitiveTypeAST *>(PrimitiveType.get())->get_type();
  auto &builder = IR::get()->getBuilder();
  //
  for (auto &var_def : VarDefs) {
    auto pairs = dynamic_cast<VarDefAST *>(var_def.get())->get_defs();
    if (!IR::get()->isGlobeBlock()) {
      auto res = builder->CreateAlloca(type, pairs.first, false);
      IR::get()->AddAlloca(res, pairs.first);
      builder->CreateStore(pairs.second, res);
    } else {
      builder->CreateGlobalVariable(IR::get()->getModule().get(), type, false,
                                    pairs.second, pairs.first);
      // IR::get()->AddGlobe(gl);
    }
  }
  return nullptr;
}
