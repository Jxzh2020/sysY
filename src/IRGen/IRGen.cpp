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
        return IRBase::CreateIRBase(IR_GLOBAL_VAR, res->second.get());
}

void Module::add_func(const std::string& name, Function *F) {
    if( this->func_list.find(name) != this->func_list.end()){
        std::cout << "Function already exists!" << std::endl;
        exit(1);
    }
    this->func_list.insert(std::pair<std::string, std::unique_ptr<Function>>( name,std::unique_ptr<Function>(F)));
}

Module::Module(const std::string& name): module_name(name) {
    // complete
}

Function *Module::get_function(const std::string &name) {
    if(this->func_list.find(name) == this->func_list.end())
        return nullptr;
    return this->func_list[name].get();
}

void Module::add_variable(GlobalVariable *var) {
    if(this->global_var_list.find(var->get_name()) != this->global_var_list.end()){
        std::cout << "GlobalVariable name repetition!" << std::endl;
        exit(1);
    }
    this->global_var_list[var->get_name()] = std::unique_ptr<GlobalVariable>(var);
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

bool Type::isInt32(Type *ty) {
    return ty == allocated[INT32].get();
}

bool Type::isInt1(Type *ty) {
    return ty == allocated[INT1].get();
}

bool Type::isVoid(Type *ty) {
    return ty == allocated[VOID].get();
}

Inst *Type::Cast(Constant *from, Type *to_type) {
    return nullptr;
}

Inst *Type::Cast(Inst *from_inst, Type *to_type) {
    return nullptr;
}

Inst *Type::Cast(IRBase *from, Type *to) {
    assert(0);
    return nullptr;
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

std::vector<std::unique_ptr<IRBase> > IRBase::base_list;

IRBase::IRBase(IR_TYPE type, Inst *val): inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr), global(nullptr), type(nullptr) {
    ir_type = type;
    inst = val;

    this->type = val->get_type();
}

IRBase::IRBase(IR_TYPE type, Constant *val): inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr), global(nullptr), type(nullptr) {
    ir_type = IR_VALUE;
    constant = val;

    this->type = val->get_type();
}

IRBase::IRBase(IR_TYPE type, Alloca *val): inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr), global(nullptr), type(nullptr) {
    ir_type = IR_ALLOCA;
    allo = val;

    this->type = val->get_type();
}

IRBase::IRBase(IR_TYPE type, Arg *val): inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr), global(nullptr), type(nullptr) {
    ir_type = IR_ARG;
    arg = val;

    this->type = val->get_type();
}

IRBase::IRBase(IR_TYPE type, GlobalVariable *val): inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr), global(nullptr), type(nullptr) {
    ir_type = IR_GLOBAL_VAR;

    global = val;
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
    return allo;
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
            return allo->get_name();
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
    auto tmp = new IRBase(type, val);
    IRBase::base_list.push_back(std::unique_ptr<IRBase>(tmp));
    return tmp;
}

std::string IRBase::get_value() const {
    switch(this->ir_type){
        case IR_VALUE:
            return this->constant->get_value();
        case IR_INST:
            return this->inst->get_value();
        case IR_ALLOCA:
            assert(0);
            return this->allo->get_value();
    }
}

IRBase *IRBase::CreateIRBase(IR_TYPE type, Arg *val) {
    auto tmp = new IRBase(type, val);
    IRBase::base_list.push_back(std::unique_ptr<IRBase>(tmp));
    return tmp;
}

IR_TYPE IRBase::get_ir_type() const {
    return this->ir_type;
}

IRBase *IRBase::CreateIRBase(IR_TYPE type, GlobalVariable *val) {
    auto tmp = new IRBase(type, val);
    IRBase::base_list.push_back(std::unique_ptr<IRBase>(tmp));
    return tmp;
}




std::vector<std::unique_ptr<FunctionType>> FunctionType::list;

std::vector<Type *> &FunctionType::get_params() {
    return this->params;
}

Type *FunctionType::get_ret_type() {
    return this->ret;
}

FunctionType *FunctionType::get(Type *ret_ty, std::vector<Type *> params) {
    auto tmp = new FunctionType(ret_ty, params);
    FunctionType::list.push_back(std::unique_ptr<FunctionType>(tmp));
    return tmp;
}

FunctionType *FunctionType::get(Type *ret_ty) {
    auto tmp = new FunctionType(ret_ty);
    FunctionType::list.push_back(std::unique_ptr<FunctionType>(tmp));
    return tmp;
}

FunctionType::FunctionType(Type *ret_ty, std::vector<Type *> &params) : params(params), ret(ret_ty) {
    // complete
}

FunctionType::FunctionType(Type *ret_ty): ret(ret_ty) {
    // complete
}


Function *Function::Create(FunctionType *ty, Linkage link, const std::string &name, Module *module) {
    auto func = new Function(ty, name, link);
    module->add_func(name, func);
    return func;
}


const std::string &Function::get_name() const {
    return this->name;
}

bool Function::arg_empty() const {
    return this->arg_list.empty();
}

std::vector<std::unique_ptr<Arg> >::iterator Function::arg_begin() {
    return this->arg_list.begin();
}

std::vector<std::unique_ptr<Arg> >::iterator Function::arg_end() {
    return this->arg_list.end();
}


void Function::add_block(BasicBlock * b) {
    if( this->b_list.find(b->get_name()) != this->b_list.end()){
        std::cout << "BasicBlocks name repetition." << std::endl;
        exit(1);
    }
    this->b_list.insert(std::pair<std::string, std::unique_ptr<BasicBlock>>(b->get_name(),std::unique_ptr<BasicBlock>(b)));

}

Linkage Function::ExternalLinkage = ExternalLinkage;
Linkage Function::InternalLinkage = InternalLinkage;

Function::Function(FunctionType *ty, const std::string &_name, Linkage _link):
    link(_link), v_reg_assigned(0), type(ty), name(_name) {
    for( auto &i: ty->get_params()){
        arg_list.push_back(std::make_unique<Arg>(i));
    }
}

unsigned int &Function::get_reg() {
    return this->v_reg_assigned;
}


const std::string &Arg::get_name() {
    return this->name;
}

void Arg::set_name(const std::string& n) {
    this->name = n;
}

Arg::Arg(Type *ty) {
    this->type = ty;
}

Type *Arg::get_type() const {
    return this->type;
}


BasicBlock::BasicBlock(const std::string &_name, Function *func) : name(_name), function(func) {
    insert_point = inst_list.end();
    // complete
}
BasicBlock *BasicBlock::Create(const std::string &name, Function *function) {
    auto tmp = new BasicBlock(name, function);
    function->add_block(tmp);
    return tmp;
}

const std::string &BasicBlock::get_name() const {
    return this->name;
}

void BasicBlock::insert(Inst *inst) {
    inst_list.insert(insert_point, inst);
}

unsigned int &BasicBlock::get_func_reg() {
    assert(this->function);
    return this->function->get_reg();
}


std::list<std::unique_ptr<Alloca>> Alloca::alloca_list;

Alloca::Alloca(Type *ty, const std::string &_name):  isConst(false), con_ptr(nullptr), type(ty), name(_name), v_reg(0) {}

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

void Alloca::set_value(IRBase *val) {
    auto cons = val->dyn_cast<Constant*>();
    auto inst = val->dyn_cast<Inst*>();
    if(cons){
        this->isConst = true;
        this->con_ptr = cons;
        this->value = this->con_ptr->get_value();
    }
    else{
        // necessary, alloca from const to v_reg
        this->isConst = false;
        this->con_ptr = nullptr;
        this->value = val->get_value();
    }
}

std::string Alloca::get_value() {

    return this->value;
}

bool Alloca::isConstant() const {
    return this->isConst;
}

Constant *Alloca::get_con_ptr() {
    assert(this->con_ptr);
    return this->con_ptr;
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

Constant *Constant::Create(CMP_TYPE op, Constant *lhs, Constant *rhs) {
    int l = Type::isInt32(lhs->get_type()) ? lhs->value.int_val : lhs->value.bool_val;
    int operand = rhs != nullptr ? Type::isInt32(rhs->get_type()) ? rhs->value.int_val : rhs->value.bool_val : 0;
    bool res;
    switch(op){
        case CMP_EQ:
            res = l == operand;
            break;
        case CMP_NE:
            res = l != operand;
            break;
        case CMP_LT:
            res = l < operand;
            break;
        case CMP_LE:
            res = l <= operand;
            break;
        case CMP_GT:
            res = l > operand;
            break;
        case CMP_GE:
            res = l >= operand;
            break;
    }
    return Constant::get(Type::getInt1(), res)->dyn_cast<Constant*>();
}

Constant *Constant::Create(LG_TYPE op, Constant *lhs, Constant *rhs) {
    auto l = Type::isInt32(lhs->get_type()) ? lhs->value.int_val : lhs->value.bool_val;
    auto operand = rhs != nullptr ? Type::isInt32(rhs->get_type()) ? rhs->value.int_val : rhs->value.bool_val : 0;
    bool res;
    switch(op){
        case LG_AND:
            res = l && operand;
            break;
        case LG_OR:
            res = l || operand;
            break;
    }
    return Constant::get(Type::getInt1(), res)->dyn_cast<Constant*>();
}

std::string Constant::get_value() {
    switch(this->type->get_type()){
        case INT32:
            return std::to_string(this->value.int_val);
        case INT1:
            return this->value.bool_val ? "true" : "false";
        default:
            std::cout << "Constant of non-int32 type!" << std::endl;
            exit(1);
    }
}

IRBase *GlobalVariable::Create(Module *module, Type *ty, bool constant, IRBase *val, const std::string &name) {
    auto globe = new GlobalVariable(ty, constant, val, name);
    module->add_variable(globe);
    return IRBase::CreateIRBase(IR_GLOBAL_VAR, globe);
}

Type *GlobalVariable::get_type() {
    return this->type;
}

const std::string &GlobalVariable::get_name() const {
    return this->name;
}

bool GlobalVariable::isConstant() const {
    return this->isConst;
}

GlobalVariable::GlobalVariable(Type *ty, bool constant, IRBase *_val, const std::string &ident): type(ty), isConst(constant), val(_val), name(ident) {
    // complete
}
