//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/VarDefAST.h"
#include "Ast/BaseAST.h"
#include "Ast/ExpAST.h"
#include <string>
#include <vector>

std::string VarDefAST::astJson(int size) {
    // std::string ident;
    // std::unique_ptr<BaseAST> ConstExp;
    // std::unique_ptr<BaseAST> InitVal;
    std::vector<std::string> children;
    children.push_back(Json("Variable", {Escape(ident)}, sizeplus(size)));
    if (InitVal != nullptr) {
        if (ConstExp != nullptr) {
            children.push_back(Json("Array Index", {ConstExp->astJson(sizeplus(size))}, sizeplus(size)));
        }
        children.push_back(Json("Init Value", {InitVal->astJson(sizeplus(size))}, sizeplus(size)));
    } else if (ConstExp != nullptr) {
        children.push_back(Json("Array Index", {ConstExp->astJson(sizeplus(size))}, sizeplus(size)));
    }
    return Json("Variable Define", children, size);
}

IRGen::IRBase *VarDefAST::codegen() {
    // deleted
    auto &builder = IR::get()->getBuilder();
    auto down = InitVal ? InitVal->codegen() : nullptr;

    //************
    if (!IR::get()->isGlobeBlock()) // local
    {
        if (ConstExp == nullptr) // local variable
        {
            // TODO: unknown_usage
            auto res = builder->CreateAlloca(this->typeast->get_type(), ident, false);
            IR::get()->AddAlloca(res, ident);
            if (down)
                builder->CreateStore(down, res);
            return res;
        } else // local array
        {
            auto index = ConstExp->codegen();
            auto size = std::atoi(index->dyn_cast<IRGen::Constant *>()->get_value().c_str());

            auto arraySize = index;

            auto arrayType = IRGen::Type::getArray(this->typeast->get_type(), size);

            auto res = builder->CreateAlloca(arrayType, ident, false);
            IR::get()->AddAlloca(res, ident);

            if (InitVal) {
                // Create a constant integer with value 0
                auto zero = IRGen::Constant::get(IRGen::Type::getInt32(), 0);

                for (int j = 0; j < size; j++) {
                    // Get a pointer to the j-th element of the array

                    auto elemPtr = builder->CreateGEP(
                            res->get_type(), res,
                            {zero, IRGen::Constant::get(IRGen::Type::getInt32(), j)});
                    // Create a constant for the initial value of the j-th element
                    auto initValRawPtr = InitVal.get();
                    auto initValPtr = dynamic_cast<ExpAST *>(initValRawPtr);

                    // Store the initial value into the j-th element
                    builder->CreateStore(initValPtr->vals[j]->codegen(), elemPtr);
                }
            }

            // builder->CreateStore(res, res);
            return res;
        }
    } else // global
    {
//        assert(0 && "Global Array Not Implemented");
//        return nullptr;
         if (ConstExp.get() == nullptr) // global variable
         {
           // TODO: unknown_usage
           builder->CreateGlobalVariable(IR::get()->getModule().get(), down->get_type(), false,down, ident);
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

std::pair<const std::string &, IRGen::IRBase *> VarDefAST::get_defs() const {
    // -> IDENT
    if (InitVal == nullptr) {
        return std::pair<const std::string &, IRGen::IRBase *>{ident, nullptr};
    }
        // -> IDENT = InitVal
    else {
        return std::pair<const std::string &, IRGen::IRBase *>{ident, InitVal->codegen()};
    }

}

void VarDefAST::set_type(PrimitiveTypeAST *src) { this->typeast = src; }
