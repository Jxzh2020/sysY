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

void Module::add_func(const std::string& name, Function *F) {
    if( this->func_list.find(name) != this->func_list.end()){
        std::cout << "Function already exists!" << std::endl;
        exit(1);
    }
    this->func_list.insert(std::pair<std::string, std::unique_ptr<Function>>( name,std::unique_ptr<Function>(F)));
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

Type *Constant::get_type() const {
    return type;
}

const std::string &Constant::get_name() const {
    return name;
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

IRBase::IRBase(IR_TYPE type, Inst *val) {
    ir_type = type;
    inst = val;
    constant = nullptr;
    alloca = nullptr;
    this->type = nullptr;
}

IRBase::IRBase(IR_TYPE type, Constant *val) {
    ir_type = IR_VALUE;
    constant = val;
    inst = nullptr;
    alloca = nullptr;
    this->type = val->get_type();
}

IRBase::IRBase(IR_TYPE type, Alloca *val) {
    ir_type = IR_ALLOCA;
    alloca = val;
    constant = nullptr;
    inst = nullptr;
    this->type = val->get_type();
}

IRBase *IRBase::CreateIRBase(IR_TYPE type, Inst *val) {
    auto tmp = new IRBase(type, val);
    IRBase::base_list.push_back(std::unique_ptr<IRBase>(tmp));
    return tmp;
}

IRBase *IRBase::CreateIRBase(IR_TYPE type, Constant *val) {
    auto tmp = new IRBase(type, val);
    IRBase::base_list.push_back(std::unique_ptr<IRBase>(tmp));
    return tmp;
}

Inst *IRBase::get_inst() const {
    if(ir_type != IR_INST){
        std::cout << "non-Inst IR Node interpreted as Inst IR Node!" <<std::endl;
        exit(1);
    }
    return inst;
}

Alloca *IRBase::get_alloca() const {
    if(ir_type != IR_ALLOCA){
        std::cout << "non-Alloca IR Node interpreted as Alloca IR Node!" <<std::endl;
        exit(1);
    }
    return alloca;
}

Constant *IRBase::get_constant() const {
    if(ir_type != IR_VALUE){
        std::cout << "non-Value IR Node interpreted as Value IR Node!" <<std::endl;
        exit(1);
    }
    return constant;
}

const std::string &IRBase::get_name() const {
    switch(this->ir_type){
        case IR_VALUE:
            return constant->get_name();
            break;
        case IR_ALLOCA:
            return alloca->get_name();
            break;
        case IR_INST:
            std::cout << "Error function call Inst::get_name() at IRBase::get_name()" << std::endl;
            exit(1);
            break;
    }
}

Type *IRBase::get_type() {
    return type;
}

IRBase *IRBase::CreateIRBase(IR_TYPE type, Alloca *val) {
    return nullptr;
}



template<class U>
U IRBase::dyn_cast() {
    switch(this->ir_type){
        case IR_VALUE:
            if(typeid(U) != typeid(Constant*))
                return nullptr;
            else
                return (U)constant;
            break;
        case IR_ALLOCA:
            if(typeid(U) != typeid(Alloca*))
                return nullptr;
            else
                return (U)(alloca);
            break;
        case IR_INST:
            if(typeid(U) != typeid(Inst*))
                return nullptr;
            else
                return (U)inst;
            break;
    }
    std::cout << "IRBase::dyn_cast encounters unexpected error!" << std::endl;
    exit(1);
}


std::vector<std::unique_ptr<FunctionType>> FunctionType::list;

FunctionType::FunctionType(Type *ret_ty, std::vector<Type *>& _params, bool _isVarArg):
    isVarArg(_isVarArg), params(_params), ret(ret_ty){
    // complete
}

FunctionType *FunctionType::get(Type *ret_ty, std::vector<Type *>& params, bool isVarArg) {
    auto tmp = new FunctionType(ret_ty, params, isVarArg);
    FunctionType::list.push_back(std::unique_ptr<FunctionType>(tmp));
    return tmp;
}

std::vector<Type *> &FunctionType::get_params() {
    return this->params;
}

Type *FunctionType::get_ret_type() {
    return this->ret;
}


Function *Function::Create(FunctionType *ty, const std::string &name, Module *module) {
    auto func = new Function(ty, name);
    module->add_func(name, func);
    return func;
}


const std::string &Function::get_name() const {
    return this->name;
}

bool Function::arg_empty() const {
    return this->arg_list.empty();
}

std::vector<std::unique_ptr<Function::Arg> >::iterator Function::arg_begin() {
    return this->arg_list.begin();
}

std::vector<std::unique_ptr<Function::Arg> >::iterator Function::arg_end() {
    return this->arg_list.end();
}

void Function::add_block(BasicBlock * b) {
    if( this->b_list.find(b->get_name()) != this->b_list.end()){
        std::cout << "BasicBlocks name repetition." << std::endl;
        exit(1);
    }
    this->b_list.insert(std::pair<std::string, std::unique_ptr<BasicBlock>>(b->get_name(),std::unique_ptr<BasicBlock>(b)));

}

Function::Function(FunctionType *ty, const std::string &_name):
    v_reg_assigned(0), type(ty), name(_name){
    for( auto &i: ty->get_params()){
        arg_list.push_back(std::make_unique<Arg>(i));
    }
}


const std::string &Function::Arg::get_name() {
    return this->name;
}

void Function::Arg::set_name(const std::string& n) {
    this->name = n;
}

Function::Arg::Arg(Type *ty) {
    this->type = ty;
}



BasicBlock::BasicBlock(const std::string &_name): name(_name) {
    // complete
}
BasicBlock *BasicBlock::Create(const std::string &name, Function *function) {
    auto tmp = new BasicBlock(name);
    function->add_block(tmp);
    return tmp;
}

const std::string &BasicBlock::get_name() const {
    return this->name;
}

void BasicBlock::insert(Inst *inst) {
    inst_list.insert(insert_point, inst);
}



Alloca::Alloca(Type *ty, const std::string &_name): type(ty), name(_name), v_reg(0) {}

Alloca *Alloca::Create(Type *ty, const std::string &name) {
    auto res = new Alloca(ty, name);
    Alloca::alloca_list.push_back(std::unique_ptr<Alloca>(res));
    return res;
}

/**
 * seems only impossible to executable when an alloca's lifetime comes to an end.
 * @param ptr
 */
void Alloca::Kill(Alloca *ptr) {
    auto iter = Alloca::alloca_list.begin();
    for( ; iter!= Alloca::alloca_list.end(); iter++){
        if( iter->get() == ptr){
            Alloca::alloca_list.erase(iter);
            break;
        }
    }
}

Type *Alloca::get_type() const {
    return type;
}

const std::string &Alloca::get_name() const {
    return name;
}

// TODO: only consider type of int and bool
Constant *Constant::Create(ARITH_TYPE op, Constant *lhs, Constant *rhs) {
    int res = Type::isInt32(lhs->get_type()) ? lhs->value.int_val : lhs->value.bool_val;
    int operand = rhs != nullptr ? Type::isInt32(rhs->get_type()) ? rhs->value.int_val : rhs->value.bool_val : 0;
    bool resb;
    switch(op){
        case IR_ADD:
            res = res + operand;
            break;
        case IR_SUB:
            res = res - operand;
            break;
        case IR_MUL:
            res = res * operand;
            break;
        case IR_S_DIV:
            res = res / operand;
            break;
        case IR_S_REM:
            res = res % operand;
            break;
        case IR_NEG:
            resb = (res == 0);
            return Constant::get(Type::getInt1(), resb)->dyn_cast<Constant*>();
            break;
    }
    return Constant::get(Type::getInt32(), res)->dyn_cast<Constant*>();
}
