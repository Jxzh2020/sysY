//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/FuncDefAST.h"

std::string FuncDefAST::astJson(int size) {
    // std::unique_ptr<BaseAST> func_type;
    // std::string ident;
    // std::unique_ptr<BaseAST> block;
    // std::vector<std::unique_ptr<BaseAST>> params;
    std::vector<std::string> children;
    std::vector<std::string> paras;

    children.push_back(Json("Function Type", {func_type->astJson(sizeplus(size))}, sizeplus(size)));

    children.push_back(Json("Function Name", {Escape(ident)}, sizeplus(size)));

    for (auto &p : params)
    {
        paras.push_back(p->astJson(sizeplus(size)));
    }
    children.push_back(Json("Parameters", paras, sizeplus(size)));

    children.push_back(Json("Function Block", {block->astJson(sizeplus(size))}, sizeplus(size)));

    return Json("Function Definition", children, size);
}

llvm::Value *FuncDefAST::codegen() {

    // maybe not a good idea, since arguments need alloca?
    // It seems fine to do like this, argument loading is not part of explicit basic block

    auto tmp = dynamic_cast<PrimitiveTypeAST *>(func_type.get())->get_type(
            params);    //llvm::FunctionType::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),false);

    auto F = llvm::Function::Create(tmp, llvm::Function::ExternalLinkage, this->ident, IR::get()->getModule().get());
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

//    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*IR::get()->getContext(), "entry", F);
//    llvm::ReturnInst::Create(*IR::get()->getContext(),llvm::ConstantInt::get(llvm::Type::getInt32Ty(*IR::get()->getContext()),0),BB);
    return F;
}

void FuncDefAST::SetName(llvm::Function *F) const {
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
        argIter->setName(dynamic_cast<FuncFParamAST *>(paraIter->get())->getName());
        paraIter++;
    }
}
