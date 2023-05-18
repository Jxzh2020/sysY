//
// Created by Hao Zhong on 5/16/23.
//

#include "IRGen/IRGen.h"
using namespace IRGen;

Builder::Builder() {
    this->current_at_bb = nullptr;
}

IRBase *Builder::CreateBr(BasicBlock *Des) {
    auto res = BranchInst::Create(Des);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateConBr(IRBase *condition, BasicBlock *true_b, BasicBlock *false_b) {
    auto res = BranchInst::Create(condition, true_b, false_b);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateAdd(IRBase *LHS, IRBase *RHS) {
    auto res = ArithInst::Create(IR_ADD, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateSub(IRBase *LHS, IRBase *RHS) {
    auto res = ArithInst::Create(IR_SUB, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateNeg(IRBase *LHS, IRBase *RHS) {
    auto res = ArithInst::Create(IR_NEG, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateMul(IRBase *LHS, IRBase *RHS) {
    auto res = ArithInst::Create(IR_MUL, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateSDiv(IRBase *LHS, IRBase *RHS) {
    auto res = ArithInst::Create(IR_S_DIV, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateSRem(IRBase *LHS, IRBase *RHS) {
    auto res = ArithInst::Create(IR_S_REM, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

/**
 * TODO Not completed!
 */
IRBase * Builder::CreateAlloca(Type *ty, const std::string &name) {
    auto res = AllocaInst::Create(ty, name);
}

// type checking
IRBase *Builder::CreateStore(IRBase *val, IRBase *ptr) {
    if( ptr->dyn_cast<Alloca*>() == nullptr){
        std::cout << "IRGen::IRBase::dyn_cast<Alloca*> failed, IRGen::Builder::CreateStore argument error!" << std::endl;
        exit(1);
    }
    auto res = AllocaInst::Store(val,ptr->dyn_cast<Alloca*>());
    current_at_bb->insert(res);
    return ptr;
}

IRBase *Builder::CreateLoad(IRBase *ptr, IRBase *reg) {
    if( ptr->dyn_cast<Alloca*>() == nullptr){
        std::cout << "IRGen::IRBase::dyn_cast<Alloca*> failed, IRGen::Builder::CreateStore argument error!" << std::endl;
        exit(1);
    }
    auto res = AllocaInst::Load(ptr->dyn_cast<Alloca*>(),reg);
    current_at_bb->insert(res);
    return ptr;
}

IRBase *Builder::CreateICmpSLT(IRBase *LHS, IRBase *RHS) {
    auto res = CmpInst::Create(CMP_LT, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpSGT(IRBase *LHS, IRBase *RHS) {
    auto res = CmpInst::Create(CMP_GT, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpSLE(IRBase *LHS, IRBase *RHS) {
    auto res = CmpInst::Create(CMP_LE, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpSGE(IRBase *LHS, IRBase *RHS) {
    auto res = CmpInst::Create(CMP_GE, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpEQ(IRBase *LHS, IRBase *RHS) {
    auto res = CmpInst::Create(CMP_EQ, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpNE(IRBase *LHS, IRBase *RHS) {
    auto res = CmpInst::Create(CMP_NE, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateLogicalOr(IRBase *LHS, IRBase *RHS) {
    auto res = LogicInst::Create(LG_OR, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateLogicalAnd(IRBase *LHS, IRBase *RHS) {
    auto res = LogicInst::Create(LG_AND, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateRetVoid() {
    auto res = RetInst::Create();
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST, res);
}

IRBase *Builder::CreateRet(IRBase *val) {
    if(val == nullptr){
        std::cout << "Warning: IRGen::Builder::CreateRet gets a null ptr explicitly" << std::endl;
    }
    auto res = RetInst::Create(val);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST, res);
}
