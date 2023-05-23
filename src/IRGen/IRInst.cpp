//
// Created by Hao Zhong on 5/16/23.
//

#include "IRGen/IRGen.h"

using namespace IRGen;

std::vector<std::unique_ptr<Inst> > Inst::inst_list;

bool Inst::isEvaluated() const {
    return this->evaluated;
}

unsigned int Inst::getVReg() const {
    return this->v_reg;
}

Inst::Inst(): v_reg(0), evaluated(false), isConst(false), con_ptr(nullptr) {
    // completed
}

bool Inst::isConstant() const {
    return this->isConst;
}

Constant *Inst::get_con_ptr() const {
    return this->con_ptr;
}


Inst* BranchInst::Create(BasicBlock* Des){
    auto tmp = new BranchInst(Des);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

Inst *BranchInst::Create(IRBase *con, BasicBlock *t_des, BasicBlock *f_des) {
    auto tmp = new BranchInst(con, t_des, f_des);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

BranchInst::BranchInst(BasicBlock *Des): isConBr(false), t_des(Des), f_des(nullptr), con(nullptr)  {
    // complete
}

BranchInst::BranchInst(IRBase* _con, BasicBlock *_t_des, BasicBlock *_f_des): isConBr(true), t_des(_t_des), f_des(_f_des), con(_con->dyn_cast<Inst*>()) {
    // complete
}


Inst *ArithInst::Create(ARITH_TYPE op, IRBase *_lhs, IRBase *_rhs) {
    auto tmp = new ArithInst(op, _lhs, _rhs);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

ArithInst::ArithInst(ARITH_TYPE _op, IRBase *_lhs, IRBase *_rhs): op(_op), lhs(_lhs), rhs(_rhs) {
    // complete, check constant;
    auto li = lhs->dyn_cast<Inst*>();
    auto ri = rhs->dyn_cast<Inst*>();
    auto lc = lhs->dyn_cast<Constant*>();
    auto rc = rhs->dyn_cast<Constant*>();
    if( ((li != nullptr && li->isConstant()) || lc != nullptr)
        &&
        ((ri != nullptr && ri->isConstant()) || rc != nullptr)  ){
        // is constant
        if( lc != nullptr){
            if( rc != nullptr){
                this->con_ptr = Constant::Create(_op, lc, rc);
            }
            else{
                this->con_ptr = Constant::Create(_op, lc, ri->get_con_ptr());
            }
        }
        else{
            if( rc != nullptr){
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), rc);
            }
            else{
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), ri->get_con_ptr());
            }
        }
        this->isConst = true;
    }
}



Inst *AllocaInst::Create(Type *ty, const std::string &name) {
    auto _ptr = Alloca::Create(ty, name);
    auto tmp = new AllocaInst(ALLOCA_CREATE, _ptr);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

Inst *AllocaInst::Store(IRBase *val, Alloca *ptr) {
    auto tmp = new AllocaInst(ALLOCA_STORE, ptr, val);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

Inst *AllocaInst::Load(Alloca *ptr, IRBase *val) {
    auto tmp = new AllocaInst(ALLOCA_LOAD, ptr, val);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

Alloca *AllocaInst::get_alloca() {
    return ptr;
}

AllocaInst::AllocaInst(ALLOCA_TYPE _op, Alloca *_ptr, IRBase *_val): ptr(_ptr), op(_op), val(_val) {
    // complete
}

AllocaInst::AllocaInst(ALLOCA_TYPE _op, Alloca *_ptr): ptr(_ptr), op(_op), val(nullptr) {
    // complete
}


CmpInst::CmpInst(CMP_TYPE _op, IRBase *LHS, IRBase *RHS): op(_op), lhs(LHS), rhs(RHS) {
    // complete, check constant;
    auto li = lhs->dyn_cast<Inst*>();
    auto ri = rhs->dyn_cast<Inst*>();
    auto lc = lhs->dyn_cast<Constant*>();
    auto rc = rhs->dyn_cast<Constant*>();
    if( ((li != nullptr && li->isConstant()) || lc != nullptr)
        &&
        ((ri != nullptr && ri->isConstant()) || rc != nullptr)  ){
        // is constant
        if( lc != nullptr){
            if( rc != nullptr){
                this->con_ptr = Constant::Create(_op, lc, rc);
            }
            else{
                this->con_ptr = Constant::Create(_op, lc, ri->get_con_ptr());
            }
        }
        else{
            if( rc != nullptr){
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), rc);
            }
            else{
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), ri->get_con_ptr());
            }
        }
        this->isConst = true;
    }
}

Inst *CmpInst::Create(CMP_TYPE _op, IRBase *lhs, IRBase *rhs) {
    auto res = new CmpInst(_op, lhs, rhs);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(res));
    return res;
}


LogicInst::LogicInst(LG_TYPE _op, IRBase *_lhs, IRBase *_rhs): op(_op),lhs(_lhs), rhs(_rhs) {
    // complete, check constant;
    auto li = lhs->dyn_cast<Inst*>();
    auto ri = rhs->dyn_cast<Inst*>();
    auto lc = lhs->dyn_cast<Constant*>();
    auto rc = rhs->dyn_cast<Constant*>();
    if( ((li != nullptr && li->isConstant()) || lc != nullptr)
        &&
        ((ri != nullptr && ri->isConstant()) || rc != nullptr)  ){
        // is constant
        if( lc != nullptr){
            if( rc != nullptr){
                this->con_ptr = Constant::Create(_op, lc, rc);
            }
            else{
                this->con_ptr = Constant::Create(_op, lc, ri->get_con_ptr());
            }
        }
        else{
            if( rc != nullptr){
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), rc);
            }
            else{
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), ri->get_con_ptr());
            }
        }
        this->isConst = true;
    }
}

Inst *LogicInst::Create(LG_TYPE _op, IRBase *lhs, IRBase *rhs) {
    auto res = new LogicInst(_op, lhs, rhs);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(res));
    return res;
}


Inst *RetInst::Create(IRBase *val) {
    auto res = new RetInst(val);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(res));
    return res;
}

RetInst::RetInst(IRBase *val): ret_val(val) {
    // complete
}


