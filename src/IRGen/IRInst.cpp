//
// Created by Hao Zhong on 5/16/23.
//

#include "IRGen/IRGen.h"

using namespace IRGen;

std::vector<std::unique_ptr<Inst> > Inst::inst_list;

unsigned int Inst::getVReg() const {
    return this->v_reg;
}

Inst::Inst() : v_reg(0), isConst(false), con_ptr(nullptr) {
    // completed
}

bool Inst::isConstant() const {
    return this->isConst;
}

Constant *Inst::get_con_ptr() const {
    return this->con_ptr;
}


Inst *BranchInst::Create(BasicBlock *Des) {
    auto tmp = new BranchInst(Des);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

Inst *BranchInst::Create(IRBase *con, BasicBlock *t_des, BasicBlock *f_des) {
    auto tmp = new BranchInst(con, t_des, f_des);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

BranchInst::BranchInst(BasicBlock *Des) : isConBr(false), t_des(Des), f_des(nullptr), con(nullptr) {
    // complete
}

BranchInst::BranchInst(IRBase *_con, BasicBlock *_t_des, BasicBlock *_f_des) : isConBr(true), t_des(_t_des),
                                                                               f_des(_f_des), con(_con) {
    // complete
}

Type *BranchInst::get_type() const {

    return nullptr;
}

std::string BranchInst::get_value() const {
    std::cout << " BranchInst::get_value() undefined." << std::endl;
    assert(false);
    return "";
}

Type *CallInst::get_type() const {
    return this->ret_type;
}

std::string CallInst::get_value() const {
    if (Type::isVoid(this->ret_type)) {
        std::cout << " CallInst::get_value() undefined in Void type." << std::endl;
        assert(0);
    }
    return this->output;
}

Inst *CallInst::Create(unsigned int &st, Function *func, std::vector<IRBase *> &args) {
    auto tmp = new CallInst(st, func, args);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

CallInst::CallInst(unsigned int &st, Function *_func, std::vector<IRBase *> &_args) : args(_args), func(_func),
                                                                                      ret_type(
                                                                                              _func->get_func_type()->get_ret_type()) {
    if (!Type::isVoid(this->ret_type)) {
        this->v_reg = st++;
        this->output = '%' + std::to_string(this->v_reg);
    }
}


Inst *ArithInst::Create(unsigned int &st, ARITH_TYPE op, IRBase *_lhs, IRBase *_rhs) {
    auto tmp = new ArithInst(st, op, _lhs, _rhs);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

ArithInst::ArithInst(unsigned int &st, ARITH_TYPE _op, IRBase *_lhs, IRBase *_rhs) : op(_op), lhs(_lhs), rhs(_rhs) {

    // complete, check constant;
    auto li = lhs->dyn_cast<Inst *>();
    auto ri = rhs->dyn_cast<Inst *>();
    auto lc = lhs->dyn_cast<Constant *>();
    auto rc = rhs->dyn_cast<Constant *>();
    if (((li != nullptr && li->isConstant()) || lc != nullptr)
        &&
        ((ri != nullptr && ri->isConstant()) || rc != nullptr)) {
        // is constant
        if (lc != nullptr) {
            if (rc != nullptr) {
                this->con_ptr = Constant::Create(_op, lc, rc);
            } else {
                this->con_ptr = Constant::Create(_op, lc, ri->get_con_ptr());
            }
        } else {
            if (rc != nullptr) {
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), rc);
            } else {
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), ri->get_con_ptr());
            }
        }
        this->isConst = true;
    } else {
        this->v_reg = st++;
    }
}

std::string ArithInst::get_value() const {
//    if(!this->isConst && !this->evaluated){
//        std::cout << "ArithInst not evaluated before use" << std::endl;
//        exit(1);
//    }
    if (this->isConst)
        return this->con_ptr->get_value();
    else
        return '%' + std::to_string(this->v_reg);
}

Type *ArithInst::get_type() const {
    if (this->isConst)
        return this->con_ptr->get_type();
    else
        return this->lhs->get_type();
}


Inst *AllocaInst::Create(unsigned int &st, Type *ty, const std::string &name, bool isConstant) {
    auto _ptr = Alloca::Create(ty, name, isConstant);
    auto tmp = new AllocaInst(st, ALLOCA_CREATE, _ptr);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

Inst *AllocaInst::Store(unsigned int &st, IRBase *val, Alloca *ptr, bool force) {
    auto tmp = new AllocaInst(st, ALLOCA_STORE, ptr, val, force);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));

    return tmp;
}

Inst *AllocaInst::Store(unsigned int &st, Arg *val, Alloca *ptr, bool force) {
    auto tmp = new AllocaInst(st, ALLOCA_STORE, ptr, IRBase::CreateIRBase(IR_ARG, val), force);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));

    return tmp;
}

Inst *AllocaInst::Load(unsigned int &st, Alloca *ptr) {
    auto tmp = new AllocaInst(st, ALLOCA_LOAD, ptr, nullptr,false);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

Alloca *AllocaInst::get_alloca() {
    return ptr;
}

AllocaInst::AllocaInst(unsigned int &st, ALLOCA_TYPE _op, Alloca *_ptr, IRBase *_val, bool force) : ptr(_ptr), gl_ptr(nullptr),
                                                                                        op(_op), val(_val) {

    // alloca load store
    if (Type::isPtr(_ptr->get_type()) && _ptr->get_type()->isArrayType()) {
        assert(0 && "Array type can not store or load directly");
    }

    if (_val == nullptr) {
        // load

        this->v_reg = st++;
        this->output = '%' + std::to_string(this->v_reg);
    }
        // store
    else {
        if (!force && _ptr->isInitialized() && _ptr->isConstant()) {
            std::cout << " Const Alloca Store!" << std::endl;
            assert(0);
        }
        if (_ptr->isConstant())
            _ptr->Initialize();
        this->output = _val->get_value();
    }

}

AllocaInst::AllocaInst(unsigned int &st, ALLOCA_TYPE _op, Alloca *_ptr) : ptr(_ptr), gl_ptr(nullptr), op(_op),
                                                                          val(nullptr) {
    // complete
}

std::string AllocaInst::get_value() const {
    if (this->op == ALLOCA_CREATE) {
        return this->ptr->get_value();
    } else
        return this->output;
}

Type *AllocaInst::get_type() const {
    if (this->ptr)
        return this->ptr->get_type();
    else {
        return this->gl_ptr->get_type();
    }
}

Inst *AllocaInst::Store(unsigned int &st, IRBase *val, GlobalVariable *ptr, bool force) {
    auto tmp = new AllocaInst(st, ALLOCA_STORE, ptr, val, force);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));

    return tmp;
}

Inst *AllocaInst::Store(unsigned int &st, Arg *val, GlobalVariable *ptr, bool force) {
    auto tmp = new AllocaInst(st, ALLOCA_STORE, ptr, IRBase::CreateIRBase(IR_ARG, val), force);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));

    return tmp;
}


Inst *AllocaInst::Load(unsigned int &st, GlobalVariable *ptr) {
    auto tmp = new AllocaInst(st, ALLOCA_LOAD, ptr, nullptr, false);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));

    return tmp;
}

AllocaInst::AllocaInst(unsigned int &st, ALLOCA_TYPE _op, GlobalVariable *_ptr, IRBase *_val, bool force) : ptr(nullptr),
                                                                                                gl_ptr(_ptr), op(_op),
                                                                                                val(_val) {

    // GlobalVariable load store
    if (Type::isPtr(_ptr->get_type()) && _ptr->get_type()->isArrayType()) {
        assert(0 && "Array type can not store or load directly");
    }

    if (_val == nullptr) {
        // load
        this->v_reg = st++;
        this->output = '%' + std::to_string(this->v_reg);

        if (!_ptr->isInitialized() && _ptr->isConstant()) {
            std::cout << " Const GlobalVariable Load Uninitialized!" << std::endl;
            assert(0);
        }

    }
        // store
    else {
        if (!force && _ptr->isInitialized() && _ptr->isConstant()) {
            std::cout << " Const GlobalVariable Store!" << std::endl;
            assert(0);
        }
        if (_ptr->isConstant())
            _ptr->Initialize();
        this->output = _val->get_value();
    }
}


CmpInst::CmpInst(unsigned int &st, CMP_TYPE _op, IRBase *LHS, IRBase *RHS) : op(_op), lhs(LHS), rhs(RHS) {

    // complete, check constant;
    auto li = lhs->dyn_cast<Inst *>();
    auto ri = rhs->dyn_cast<Inst *>();
    auto lc = lhs->dyn_cast<Constant *>();
    auto rc = rhs->dyn_cast<Constant *>();
    if (((li != nullptr && li->isConstant()) || lc != nullptr)
        &&
        ((ri != nullptr && ri->isConstant()) || rc != nullptr)) {
        // is constant
        if (lc != nullptr) {
            if (rc != nullptr) {
                this->con_ptr = Constant::Create(_op, lc, rc);
            } else {
                this->con_ptr = Constant::Create(_op, lc, ri->get_con_ptr());
            }
        } else {
            if (rc != nullptr) {
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), rc);
            } else {
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), ri->get_con_ptr());
            }
        }
        this->isConst = true;
    } else {
        this->v_reg = st++;
    }
}

Inst *CmpInst::Create(unsigned int &st, CMP_TYPE _op, IRBase *lhs, IRBase *rhs) {
    auto res = new CmpInst(st, _op, lhs, rhs);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(res));
    return res;
}

std::string CmpInst::get_value() const {
    if (this->isConst)
        return this->con_ptr->get_value();
    else
        return '%' + std::to_string(this->v_reg);
}

Type *CmpInst::get_type() const {
    if (this->isConst)
        return this->con_ptr->get_type();
    else
        return Type::getInt1();
}


LogicInst::LogicInst(unsigned int &st, LG_TYPE _op, IRBase *_lhs, IRBase *_rhs) : op(_op), lhs(_lhs), rhs(_rhs) {

    // complete, check constant;
    auto li = lhs->dyn_cast<Inst *>();
    auto ri = rhs->dyn_cast<Inst *>();
    auto lc = lhs->dyn_cast<Constant *>();
    auto rc = rhs->dyn_cast<Constant *>();
    if (((li != nullptr && li->isConstant()) || lc != nullptr)
        &&
        ((ri != nullptr && ri->isConstant()) || rc != nullptr)) {
        // is constant
        if (lc != nullptr) {
            if (rc != nullptr) {
                this->con_ptr = Constant::Create(_op, lc, rc);
            } else {
                this->con_ptr = Constant::Create(_op, lc, ri->get_con_ptr());
            }
        } else {
            if (rc != nullptr) {
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), rc);
            } else {
                this->con_ptr = Constant::Create(_op, li->get_con_ptr(), ri->get_con_ptr());
            }
        }
        this->isConst = true;
    } else {
        this->v_reg = st++;
    }
}

Inst *LogicInst::Create(unsigned int &st, LG_TYPE _op, IRBase *lhs, IRBase *rhs) {
    auto res = new LogicInst(st, _op, lhs, rhs);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(res));
    return res;
}

std::string LogicInst::get_value() const {
    if (this->isConst)
        return this->con_ptr->get_value();
    else
        return '%' + std::to_string(this->v_reg);
}

Type *LogicInst::get_type() const {
    if (this->isConst)
        return this->con_ptr->get_type();
    else
        return Type::getInt1();
}


Inst *RetInst::Create(IRBase *val) {
    auto res = new RetInst(val);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(res));
    return res;
}

RetInst::RetInst(IRBase *val) : ret_val(val) {
    // complete
}


Inst *GEPInst::Create(unsigned int &st, Type *aggragate_type, IRBase *arrayalloc, IRBase *base, IRBase *offset) {
    auto tmp = new GEPInst(st, aggragate_type, arrayalloc, base, offset);
    Inst::inst_list.push_back(std::unique_ptr<Inst>(tmp));
    return tmp;
}

GEPInst::GEPInst(unsigned int &st, Type *aggragate_type, IRBase *arrayalloc, IRBase *base, IRBase *offset)
        : array_type(aggragate_type), alloca_array_ptr(nullptr), gl_array_ptr(nullptr), base_index(base),
          offset_index(offset), isAlloca(true), isInst(false), alloca_load(nullptr) {

    auto alloca = arrayalloc->dyn_cast<Alloca *>();
    auto gl = arrayalloc->dyn_cast<GlobalVariable *>();


    this->v_reg = st++;
    this->output = '%' + std::to_string(this->v_reg);

    if (!alloca && arrayalloc->dyn_cast<Inst *>()) {
        this->isInst = true;
        this->alloca_load = arrayalloc->dyn_cast<Inst *>();
    } else if (alloca && Type::isPtr(alloca->get_type())) {
        this->isAlloca = true;
        this->alloca_array_ptr = alloca;
    } else if (gl && Type::isPtr(gl->get_type())) {
        this->isAlloca = false;
        this->gl_array_ptr = gl;
    } else {
        this->isAlloca = false;
        assert(0 && "Not Array Type or Even not Ptr Type");
    }

}

std::string GEPInst::get_value() const {
    return '%' + std::to_string(this->v_reg);
}

Type *GEPInst::get_type() const {
    return Type::getPtr();
    //return this->array_type->get_element_type();
}

Alloca *GEPInst::get_alloca() {
    auto isConstant = this->isInst ? dynamic_cast<AllocaInst *>(alloca_load)->get_alloca()->isConstant()
                                   : this->isAlloca ? this->alloca_array_ptr->isConstant()
                                                    : this->gl_array_ptr->isConstant();
    auto alloca = Alloca::Create(this->array_type->get_element_type(), std::to_string(this->v_reg), isConstant);
    alloca->Initialize();
    return alloca;
}



