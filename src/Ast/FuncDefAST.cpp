//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/FuncDefAST.h"

void FuncDefAST::Dump() const {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
}

IRGen::IRBase *FuncDefAST::codegen() {

    // maybe not a good idea, since arguments need alloca?
    // It seems fine to do like this, argument loading is not part of explicit basic block

    auto tmp = dynamic_cast<PrimitiveTypeAST *>(func_type.get())->get_type(
            params);    //IRGen::FunctionType::get(IRGen::Type::getInt32Ty(*IR::get()->getContext()),false);

    auto F = IRGen::Function::Create(tmp, IRGen::Function::ExternalLinkage, this->ident, IR::get()->getModule().get());
    if (block != nullptr) {
        IR::get()->EnterFunc(F);
        this->SetName(F);
        IR::get()->NewLogicalBlockStart();
        //this->SetAlloca(F);
        block->codegen();
        IR::get()->NewLogicalBlockEnd();
        IR::get()->ExitFunc();
    }
        // TODO Function Declaration
    else { ;
    }

//    IRGen::BasicBlock *BB = IRGen::BasicBlock::Create(*IR::get()->getContext(), "entry", F);
//    IRGen::ReturnInst::Create(*IR::get()->getContext(),IRGen::ConstantInt::get(IRGen::Type::getInt32Ty(*IR::get()->getContext()),0),BB);
    return nullptr;
}

void FuncDefAST::SetName(IRGen::Function *F) const {
    auto &builder = IR::get()->getBuilder();
//    for(auto& i:params){
//        builder->CreateLoad(dynamic_cast<FuncFParamAST*>(i.get())->getType(),)
//    }
    if (F->arg_empty())
        return;
    // TODO how to access parameter by its name?
    auto argIter = F->arg_begin();
    auto argEnd = F->arg_end();
    auto paraIter = params.begin();
    for (; argIter != argEnd; argIter++) {
        argIter->get()->set_name(dynamic_cast<FuncFParamAST *>(paraIter->get())->getName());
        paraIter++;
    }
}
