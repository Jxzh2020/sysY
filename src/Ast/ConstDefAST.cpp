//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstDefAST.h"
#include "Ast/BaseAST.h"
#include "Ast/ConstInitValAST.h"
#include <cstddef>
#include <string>
#include <vector>

std::string ConstDefAST::astJson(int size) {
  // std::string ident;
  // std::unique_ptr<BaseAST> ConstExp;
  // std::unique_ptr<BaseAST> ConstInitVal;
  std::vector<std::string> children;
  children.push_back(Json("identifier", {Escape(ident)}, sizeplus(size)));
  if (ConstExp == nullptr) {
    children.push_back(Json("Constant Init",
                            {ConstInitVal->astJson(sizeplus(size))},
                            sizeplus(size)));
    return Json("Constant Definition", children, size);
  } else {
    children.push_back(
        Json("Index", {ConstExp->astJson(sizeplus(size))}, sizeplus(size)));
    children.push_back(Json("Constant Init",
                            {ConstInitVal->astJson(sizeplus(size))},
                            sizeplus(size)));
    return Json("Constant Array Definition", children, size);
  }
}

IRGen::IRBase *ConstDefAST::codegen() {
  // deleted
  auto &builder = IR::get()->getBuilder();
  auto down = ConstInitVal->codegen();

  //************
  if (!IR::get()->isGlobeBlock()) // local
  {
    if (ConstExp.get() == nullptr) // local variable
    {
      // TODO: unknown_usage
      auto res = builder->CreateAlloca(down->getType(), nullptr, ident);
      builder->CreateStore(down, res);
      return res;
    } else // local array
    {
      auto index = ConstExp->codegen();

      IRGen::Constant *arraySize = llvm::dyn_cast<llvm::ConstantInt>(index);
      llvm::ArrayType *arrayType = llvm::ArrayType::get(
          /*down->getType()*/ llvm::Type::getInt32Ty(context),
          arraySize->getLimitedValue());
      llvm::AllocaInst *res = builder->CreateAlloca(arrayType, nullptr, ident);
      IR::get()->AddAlloca(res, ident);

      // Create a constant integer with value 0
      llvm::ConstantInt *zero =
          llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
      for (int j = 0; j < arraySize->getLimitedValue(); j++) {
        // Get a pointer to the j-th element of the array
        llvm::Value *indices[] = {
            zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), j)};
        llvm::Value *elemPtr =
            builder->CreateGEP(res->getAllocatedType(), res, indices);
        // Create a constant for the initial value of the j-th element
        auto initValRawPtr = ConstInitVal.get();
        auto initValPtr = dynamic_cast<ConstInitValAST *>(initValRawPtr);

        // Store the initial value into the j-th element
        builder->CreateStore(initValPtr->vals[j]->codegen(), elemPtr);
      }
      // builder->CreateStore(res, res);
      return res;
    }
  } else // global
  {
    if (ConstExp.get() == nullptr) // global variable
    {
      // TODO: unknown_usage
      auto GlobalArray =
          new llvm::GlobalVariable(*IR::get()->getModule(), down->getType(),
                                   true, llvm::GlobalValue::ExternalLinkage,
                                   llvm::dyn_cast<llvm::Constant>(down), ident);
      // IR::get()->AddGlobe(GlobalArray);
    } else // global array
    {
      auto index = ConstExp->codegen();
      llvm::LLVMContext *c = IR::get()->getContext().get();
      llvm::LLVMContext &context = *c;
      llvm::ConstantInt *arraySize = llvm::dyn_cast<llvm::ConstantInt>(index);
      llvm::ArrayType *arrayType = llvm::ArrayType::get(
          /*down->getType()*/ llvm::Type::getInt32Ty(context),
          arraySize->getLimitedValue());
      std::vector<llvm::Constant *> arrayValue;
      for (int j = 0; j < arraySize->getLimitedValue(); j++) {
        // arrayValue.push_back(llvm::ConstantInt::get((down->getType(),
        // ConstInitVal->vals[j]->codegen()->getLimitedValue())));
        arrayValue.push_back(llvm::dyn_cast<llvm::Constant>(
            dynamic_cast<ConstInitValAST *>(ConstInitVal.get())
                ->vals[j]
                ->codegen()));
      }
      llvm::Constant *initValue =
          llvm::ConstantArray::get(arrayType, arrayValue);
      auto GlobalArray = new llvm::GlobalVariable(
          *IR::get()->getModule(), arrayType, true,
          llvm::GlobalValue::ExternalLinkage, initValue, ident);
      // IR::get()->AddGlobe(GlobalArray);
    }
    return nullptr;
  }
  //************
}

std::pair<const std::string &, IRGen::IRBase *> ConstDefAST::get_defs() const {
  auto down = ConstInitVal->codegen();
  return std::pair<const std::string &, IRGen::IRBase *>{ident, down};
}
