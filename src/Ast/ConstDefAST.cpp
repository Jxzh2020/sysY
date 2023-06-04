//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/ConstDefAST.h"
#include "Ast/BaseAST.h"
#include "Ast/ConstInitValAST.h"
#include "IRGen/IRGen.h"
#include <cstddef>
#include <string>
#include <vector>

std::string ConstDefAST::astJson(int size) {
    // std::string ident;
    // std::unique_ptr<BaseAST> ConstExp;
    // std::unique_ptr<BaseAST> ConstInitVal;
    std::vector<std::string> children;
    // children.push_back(Json("identifier", {Escape(ident)}, sizeplus(size)));
    children.push_back(Json(Escape(ident), sizeplus(size)));
    if (ConstExp == nullptr) {
        // children.push_back(Json("Constant Init",
        //                         {ConstInitVal->astJson(sizeplus(size))},
        //                         sizeplus(size)));
        children.push_back(ConstInitVal->astJson(sizeplus(size)));
        return Json("Const Def", children, size);
    } else {
        children.push_back(
                Json("Index", {ConstExp->astJson(sizeplus(size))}, sizeplus(size)));
        children.push_back(Json("Constant Init",
                                {ConstInitVal->astJson(sizeplus(size))},
                                sizeplus(size)));
        return Json("Arr Define", children, size);
    }
}

IRGen::IRBase *ConstDefAST::codegen() {
    // deleted
    auto &builder = IR::get()->getBuilder();
    auto down = ConstInitVal->codegen();

    //************
    if (!IR::get()->isGlobeBlock()) // local
    {
        if (ConstExp == nullptr) // local variable
        {
            // TODO: unknown_usage
            auto res = builder->CreateAlloca(down->get_type(), ident, true);
            IR::get()->AddAlloca(res, ident);
            builder->CreateStore(down, res);
            return res;
        } else // local array
        {
            auto index = ConstExp->codegen();
            auto size = std::atoi(index->dyn_cast<IRGen::Constant *>()->get_value().c_str());

            auto arraySize = index;

            auto arrayType = IRGen::Type::getArray(this->typeast->get_type(), size);

            auto res = builder->CreateAlloca(arrayType, ident, true);
            IR::get()->AddAlloca(res, ident);

            // Create a constant integer with value 0
            auto zero = IRGen::Constant::get(IRGen::Type::getInt32(), 0);

            for (int j = 0; j < size; j++) {
                // Get a pointer to the j-th element of the array

                auto elemPtr = builder->CreateGEP(
                        res->get_type(), res,
                        {zero, IRGen::Constant::get(IRGen::Type::getInt32(), j)});
                // Create a constant for the initial value of the j-th element
                auto initValRawPtr = ConstInitVal.get();
                auto initValPtr = dynamic_cast<ConstInitValAST *>(initValRawPtr);

                // Store the initial value into the j-th element
                builder->CreateStore(initValPtr->vals[j]->codegen(), elemPtr, true);
            }
            // builder->CreateStore(res, res);
            return res;
        }
    } else // global
    {
        if (ConstExp.get() == nullptr) // global variable
        {
            // TODO: unknown_usage
            builder->CreateGlobalVariable(IR::get()->getModule().get(), down->get_type(), true,down, ident);
            // IR::get()->AddGlobe(GlobalArray);
        } else // global array
        {
            assert(0 && "Global Array Not Implemented");
//        return nullptr;
//           auto index = ConstExp->codegen();
//           auto arraySize = index->dyn_cast<IRGen::Constant*>();
//           auto *arrayType = IRGen::Type::getArray(this->typeast->get_type(), std::atoi(arraySize->get_value().c_str()));
//
//           std::vector<llvm::Constant *> arrayValue;
//           for (int j = 0; j < arraySize->getLimitedValue(); j++) {
//             // arrayValue.push_back(llvm::ConstantInt::get((down->getType(),
//             // ConstInitVal->vals[j]->codegen()->getLimitedValue())));
//             arrayValue.push_back(llvm::dyn_cast<llvm::Constant>(
//                 dynamic_cast<ConstInitValAST *>(ConstInitVal.get())
//                     ->vals[j]
//                     ->codegen()));
//           }
//           llvm::Constant *initValue =
//               llvm::ConstantArray::get(arrayType, arrayValue);
//           auto GlobalArray = new llvm::GlobalVariable(
//               *IR::get()->getModule(), arrayType, true,
//               llvm::GlobalValue::ExternalLinkage, initValue, ident);
//           // IR::get()->AddGlobe(GlobalArray);
        }
        return nullptr;
    }
    //************
}

std::pair<const std::string &, IRGen::IRBase *> ConstDefAST::get_defs() const {
    auto down = ConstInitVal->codegen();
    return std::pair<const std::string &, IRGen::IRBase *>{ident, down};
}

void ConstDefAST::set_type(PrimitiveTypeAST *src) { this->typeast = src; }