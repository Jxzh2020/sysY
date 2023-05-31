//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstInitValAST.h"
#include "Ast/BaseAST.h"
#include "IRGen/IRGen.h"
#include <string>
#include <vector>

std::string ConstInitValAST::astJson(int size) {
  // std::unique_ptr<BaseAST> ConstExp;
  // std::vector<std::unique_ptr<BaseAST>> vals;

  if (vals.empty()) {
    return Json("Constant Initiate", {ConstExp->astJson(sizeplus(size))}, size);
  } else {
    std::vector<std::string> children;
    for (auto &v : vals) {
      children.push_back(v->astJson(sizeplus(size)));
    }
    return Json("Constant Array Initiate", {children}, size);
  }
}

IRGen::IRBase *ConstInitValAST::codegen() {
  if (vals.empty()) {
    return ConstExp->codegen();
  }
  return nullptr;
}
