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


// TODO: Refactor starts from here


IRBase *Builder::CreateAdd(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = ArithInst::Create(st, IR_ADD, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateSub(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = ArithInst::Create(st, IR_SUB, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateNeg(IRBase *LHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = ArithInst::Create(st, IR_NEG, LHS, nullptr);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateMul(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = ArithInst::Create(st, IR_MUL, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateSDiv(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = ArithInst::Create(st, IR_S_DIV, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateSRem(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = ArithInst::Create(st, IR_S_REM, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

/**
 * TODO Not completed!
 */
IRBase * Builder::CreateAlloca(Type *ty, const std::string &name, bool isConstant) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    int cnt = 0;
    auto ret = this->current_at_bb->get_func()->get_alloca(name);
    Inst* res;

    while(ret){
        cnt++;
        ret = this->current_at_bb->get_func()->get_alloca(name+std::to_string(cnt));
    }
    if(cnt)
        res = AllocaInst::Create(st, ty, name+std::to_string(cnt), isConstant);
    else
        res = AllocaInst::Create(st, ty, name, isConstant);
    this->current_at_bb->get_func()->add_alloca(dynamic_cast<AllocaInst*>(res)->get_alloca());
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

// type checking
IRBase *Builder::CreateStore(IRBase *val, IRBase *ptr) {
    Inst* res;
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    if( ptr->dyn_cast<Alloca*>() == nullptr){
        if( dynamic_cast<AllocaInst*>(ptr->dyn_cast<Inst*>()) == nullptr){
            std::cout << "IRGen::IRBase::dyn_cast<Alloca*> failed, IRGen::Builder::CreateStore argument error!" << std::endl;
            exit(1);
        }
        res = AllocaInst::Store(st, val,dynamic_cast<AllocaInst*>(ptr->dyn_cast<Inst*>())->get_alloca());
        current_at_bb->insert(res);
        return IRBase::CreateIRBase(IR_INST, res);
    }
    else{
        res = AllocaInst::Store(st, val,ptr->dyn_cast<Alloca*>());
    }
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST, res);
}

IRBase *Builder::CreateLoad(IRBase *ptr) {
    Inst* res;
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    if( ptr->dyn_cast<Alloca*>() == nullptr){
        if( dynamic_cast<AllocaInst*>(ptr->dyn_cast<Inst*>()) == nullptr){
            std::cout << "IRGen::IRBase::dyn_cast<Alloca*> failed, IRGen::Builder::CreateStore argument error!" << std::endl;
            exit(1);
        }
        res = AllocaInst::Load(st, dynamic_cast<AllocaInst*>(ptr->dyn_cast<Inst*>())->get_alloca());
        current_at_bb->insert(res);
        return IRBase::CreateIRBase(IR_INST, res);
    }
    res = AllocaInst::Load(st, ptr->dyn_cast<Alloca*>());
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST, res);;
}

IRBase *Builder::CreateICmpSLT(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = CmpInst::Create(st, CMP_LT, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpSGT(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = CmpInst::Create(st, CMP_GT, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpSLE(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = CmpInst::Create(st, CMP_LE, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpSGE(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = CmpInst::Create(st, CMP_GE, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpEQ(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = CmpInst::Create(st, CMP_EQ, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateICmpNE(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = CmpInst::Create(st, CMP_NE, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateLogicalOr(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = LogicInst::Create(st, LG_OR, LHS, RHS);
    current_at_bb->insert(res);
    return IRBase::CreateIRBase(IR_INST,res);
}

IRBase *Builder::CreateLogicalAnd(IRBase *LHS, IRBase *RHS) {
    auto& st = this->current_at_bb->get_func_reg();
    current_at_bb->set_v_reg_range(st);
    auto res = LogicInst::Create(st, LG_AND, LHS, RHS);
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

IRBase *Builder::CreateCall(Function *fun) {
    assert(false);
    return nullptr;
}

IRBase *Builder::CreateCall(Function *fun, std::vector<IRBase *> args) {
    assert(false);
    return nullptr;
}

BasicBlock *Builder::SetInsertPoint(BasicBlock *basicblock) {
    this->current_at_bb = basicblock;
    return nullptr;
}
