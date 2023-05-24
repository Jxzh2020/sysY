//
// Created by Hao Zhong on 5/16/23.
//

#include "IRGen/IRGen.h"

using namespace IRGen;

std::vector<std::unique_ptr<Inst> > Inst::inst_list;

unsigned int Inst::getVReg() const {
    return this->v_reg;
}

Inst::Inst(): v_reg(0), isConst(false), con_ptr(nullptr) {
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

BranchInst::BranchInst(IRBase* _con, BasicBlock *_t_des, BasicBlock *_f_des): isConBr(true), t_des(_t_des), f_des(_f_des), con(_con) {
    // complete
}

Type *BranchInst::get_type() const {

    return nullptr;
}

std::string BranchInst::get_value() const {
    assert(false);
    return "";
}


Inst *ArithInst::Create(unsigned int &st, ARITH_TYPE op, IRBase *_lhs, IRBase *_rhs) {
    auto tmp = new ArithInst(st, op, _lhs, _rhs);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

ArithInst::ArithInst(unsigned int& st, ARITH_TYPE _op, IRBase *_lhs, IRBase *_rhs): op(_op), lhs(_lhs), rhs(_rhs) {

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
    else{
        this->v_reg = st++;
    }
}

std::string ArithInst::get_value() const {
//    if(!this->isConst && !this->evaluated){
//        std::cout << "ArithInst not evaluated before use" << std::endl;
//        exit(1);
//    }
    if(this->isConst)
        return this->con_ptr->get_value();
    else
        return '%'+std::to_string(this->v_reg);
}

Type *ArithInst::get_type() const {
    if(this->isConst)
        return this->con_ptr->get_type();
    else
        return this->lhs->get_type();
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

    ptr->set_value(val);

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
    this->output = _ptr->get_value();
}

AllocaInst::AllocaInst(ALLOCA_TYPE _op, Alloca *_ptr): ptr(_ptr), op(_op), val(nullptr) {
    // complete
}

std::string AllocaInst::get_value() const {
    if(this->isConst)
        return this->con_ptr->get_value();
    else
        return this->output;
}

Type *AllocaInst::get_type() const {
    return this->ptr->get_type();
}


CmpInst::CmpInst(unsigned int& st, CMP_TYPE _op, IRBase *LHS, IRBase *RHS): op(_op), lhs(LHS), rhs(RHS) {

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
    else{
        this->v_reg = st++;
    }
}

Inst *CmpInst::Create(unsigned int& st, CMP_TYPE _op, IRBase *lhs, IRBase *rhs) {
    auto res = new CmpInst(st, _op, lhs, rhs);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(res));
    return res;
}

std::string CmpInst::get_value() const {
    if(this->isConst)
        return this->con_ptr->get_value();
    else
        return '%'+std::to_string(this->v_reg);
}

Type *CmpInst::get_type() const {
    if(this->isConst)
        return this->con_ptr->get_type();
    else
        return Type::getInt1();
}


LogicInst::LogicInst(unsigned int& st, LG_TYPE _op, IRBase *_lhs, IRBase *_rhs): op(_op),lhs(_lhs), rhs(_rhs) {

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
    else{
        this->v_reg = st++;
    }
}

Inst *LogicInst::Create(unsigned int& st, LG_TYPE _op, IRBase *lhs, IRBase *rhs) {
    auto res = new LogicInst(st, _op, lhs, rhs);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(res));
    return res;
}

std::string LogicInst::get_value() const {
    if(this->isConst)
        return this->con_ptr->get_value();
    else
        return '%' + std::to_string(this->v_reg);
}

Type *LogicInst::get_type() const {
    if(this->isConst)
        return this->con_ptr->get_type();
    else
        return Type::getInt1();
}


Inst *RetInst::Create(IRBase *val) {
    auto res = new RetInst(val);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(res));
    return res;
}

RetInst::RetInst(IRBase *val): ret_val(val) {
    // complete
}


