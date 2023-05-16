//
// Created by Hao Zhong on 5/15/23.
//

#include "IRGen/IRGen.h"
using namespace IRGen;

IRBase *Module::getGlobalVariable(const std::string &name) const {
    auto res = this->global_var_list.find(name);
    if( res == this->global_var_list.end())
        return nullptr;
    else
        return res->second.get();
}

std::vector<std::unique_ptr<Type> > Type::allocated;

Type *IRGen::Type::getInt32() {
    if( Type::allocated.empty()){
        Type::gen_all_instances();
    }
    return Type::allocated[INT32].get();
}

Type *IRGen::Type::getInt1() {
    if( Type::allocated.empty()){
        Type::gen_all_instances();
    }
    return Type::allocated[INT1].get();
}

Type *IRGen::Type::getVoid() {
    if( Type::allocated.empty()){
        Type::gen_all_instances();
    }
    return Type::allocated[VOID].get();
}

Type::Type(P_TYPE _type) {
    type = _type;
}

/**
 *  Note, in order to implement the case that allocated[INT32].get == Type/(int32), the push_back order should
 *  be the same as P_TYPE order.
 */
void Type::gen_all_instances() {
    auto gen = new Type(INT32);
    Type::allocated.push_back(std::unique_ptr<Type>(gen));
    gen = new Type(INT1);
    Type::allocated.push_back(std::unique_ptr<Type>(gen));
    gen = new Type(VOID);
    Type::allocated.push_back(std::unique_ptr<Type>(gen));
}

P_TYPE Type::get_type() {
    return type;
}

std::vector<std::unique_ptr<Constant> > Constant::const_list;
Constant::Constant(Type *ty) {
    type = ty;
}

Type *Constant::get_type() {
    return type;
}

template<class T>
IRBase *Constant::get(Type *ty, T val) {
    auto tmp = new Constant(ty);
    Constant::const_list.push_back(std::unique_ptr<Constant>(tmp));
    switch(ty->get_type()){
        case INT32:
            tmp->value.int_val = val;
            break;
        case INT1:
            tmp->value.bool_val = val;
            break;
        case VOID:
            std::cout << "Unexpected Error! Constant cannot be void type." << std::endl;
            exit(1);
            break;
    }
    auto res = IRBase::CreateIRBase(IR_VALUE, tmp);

    return res;
}

IRBase::IRBase(Inst *val) {
    ir_type = IR_INST;
    inst = val;
    constant = nullptr;
    type = nullptr;
}

IRBase::IRBase(Constant *val) {
    ir_type = IR_VALUE;
    constant = val;
    inst = nullptr;
    type = val->get_type();
}
