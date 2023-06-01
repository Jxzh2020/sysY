//
// Created by Hao Zhong on 4/21/23.
//

#include "Ast/LValAST.h"
#include "Ast/BaseAST.h"
#include "IRGen/IRGen.h"
#include <string>
#include <vector>

std::string LValAST::astJson(int size) {
    // std::string ident;
    // std::unique_ptr<BaseAST> Exp;
    return Json("Load Value", {Escape(ident)}, size);
}

IRGen::IRBase *LValAST::codegen() {
    auto &builder = IR::get()->getBuilder();
    auto ref = IR::get()->GetAlloca(ident);

    if (ref == nullptr) {
        ref = IR::get()->getModule()->getGlobalVariable(ident);
    }
    // not found
    if (ref == nullptr) {
        std::cout << "Error! LVal not found!" << std::endl;
        exit(1);
    }
    if (this->Exp) { // array
        auto iter = IR::get()->getFunc()->arg_begin();
        for (; iter != IR::get()->getFunc()->arg_end(); iter++) {
            if (iter->get()->get_name() == this->ident)
                ref = builder->CreateLoad(ref);
        }
        ref = builder->CreateGEP(ref->get_type(), ref,
                                 {IRGen::Constant::get(IRGen::Type::getInt32(), 0),
                                  this->Exp->codegen()});
    }
    return ref;
}
