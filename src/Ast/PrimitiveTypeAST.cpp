//
// Created by Hao Zhong on 4/28/23.
//

#include "Ast/PrimitiveTypeAST.h"

std::string PrimitiveTypeAST::astJson(int size) {
  // std::string type;

  return Json("Type", {Escape(type)}, size);
}

llvm::Type *PrimitiveTypeAST::get_type() const {
  if (type == "int")
    return IRGen::Type::getInt32();
  else if (type == "void")
    return IRGen::Type::getVoid();
  else
    return nullptr;
}

IRGen::FunctionType *PrimitiveTypeAST::get_type(
    const std::vector<std::unique_ptr<BaseAST>> &params) const {
  // TODO FuncFParam AST push_back inside loop, fixed
  std::vector<IRGen::Type *> types;
  types.reserve(params.size());
  for (auto &i : params) {
    types.emplace_back(dynamic_cast<FuncFParamAST *>(i.get())->getType());
  }
  if (type == "int")
    return IRGen::FunctionType::get(IRGen::Type::getInt32(), types);
  else if (type == "void")
    return IRGen::FunctionType::get(IRGen::Type::getVoid(), types);
  else
    return nullptr;
}
