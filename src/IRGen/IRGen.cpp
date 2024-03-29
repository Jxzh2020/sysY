//
// Created by Hao Zhong on 5/15/23.
//

#include "IRGen/IRGen.h"

using namespace IRGen;


IRBase *Module::getGlobalVariable(const std::string &name) const {
    auto res = this->global_var_list.find(name);
    if (res == this->global_var_list.end())
        return nullptr;
    else
        return IRBase::CreateIRBase(IR_GLOBAL_VAR, res->second.get());
}

void Module::add_func(const std::string &name, Function *F) {
    for (auto &func: this->func_list)
        if (func->get_name() == name) {
            std::cout << "Function already exists!" << std::endl;
            exit(1);
        }
    this->func_list.insert(this->func_list.end(), std::unique_ptr<Function>(std::unique_ptr<Function>(F)));
}

Module::Module(const std::string &name) : module_name(name) {
    // complete
}

Function *Module::get_function(const std::string &name) {
    for (auto &func: this->func_list)
        if (func->get_name() == name)
            return func.get();
    return nullptr;
}

void Module::add_variable(GlobalVariable *var) {
    if (this->global_var_list.find(var->get_name()) != this->global_var_list.end()) {
        std::cout << "GlobalVariable name repetition!" << std::endl;
        exit(1);
    }
    this->global_var_list[var->get_name()] = std::unique_ptr<GlobalVariable>(var);
}


Type *IRGen::Type::getInt32() {
    if (Type::allocated.empty()) {
        Type::gen_all_instances();
    }
    return Type::allocated[INT32].get();
}

Type *IRGen::Type::getInt1() {
    if (Type::allocated.empty()) {
        Type::gen_all_instances();
    }
    return Type::allocated[INT1].get();
}

Type *IRGen::Type::getVoid() {
    if (Type::allocated.empty()) {
        Type::gen_all_instances();
    }
    return Type::allocated[VOID].get();
}

Type *IRGen::Type::getPtr() {
    if (Type::allocated.empty()) {
        Type::gen_all_instances();
    }
    return Type::allocated[PTR].get();
}


Type *IRGen::Type::getArray(Type *ty, unsigned int arraysize) {
    auto res = new Type(ty, arraysize);
    Type::compound.push_back(std::unique_ptr<Type>(res));

    return res;
}

Type::Type(P_TYPE _type) : type(_type), isArray(false), arraysize(0), ele_type(_type) {
    // complete
}

Type::Type(Type *_type, unsigned int _arraysize) : type(_arraysize != 0 ? PTR : _type->get_type()),
                                                   isArray(_arraysize != 0), arraysize(_arraysize),
                                                   ele_type(_type->get_type()) {
    // complete
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
    gen = new Type(PTR);
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

bool Type::isPtr(Type *ty) {
    //return ty == allocated[PTR].get();
    return ty->type == PTR;
}

Inst *Type::Cast(Constant *from, Type *to_type) {
    assert(0 && "Type::Cast use before definition");
    return nullptr;
}

Inst *Type::Cast(Inst *from_inst, Type *to_type) {
    assert(0 && "Type::Cast use before definition");
    return nullptr;
}

Inst *Type::Cast(IRBase *from, Type *to) {
    assert(0 && "Type::Cast use before definition");
    return nullptr;
}

bool Type::isArrayType() const {
    return this->isArray;
}

Type *Type::get_element_type() const {
    assert(this->isArray && "Getting ElementType from Non-Array Type");
    if (Type::allocated.empty()) {
        Type::gen_all_instances();
    }
    return Type::allocated[this->ele_type].get();
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
    switch (ty->get_type()) {
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

IRBase::IRBase(IR_TYPE type, Inst *val) : inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr),
                                          global(nullptr), type(nullptr) {
    ir_type = type;
    inst = val;

    this->type = val->get_type();
}

IRBase::IRBase(IR_TYPE type, Constant *val) : inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr),
                                              global(nullptr), type(nullptr) {
    ir_type = IR_VALUE;
    constant = val;

    this->type = val->get_type();
}

IRBase::IRBase(IR_TYPE type, Alloca *val) : inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr),
                                            global(nullptr), type(nullptr) {
    ir_type = IR_ALLOCA;
    allo = val;

    this->type = val->get_type();
}

IRBase::IRBase(IR_TYPE type, Arg *val) : inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr), global(nullptr),
                                         type(nullptr) {
    ir_type = IR_ARG;
    arg = val;

    this->type = val->get_type();
}

IRBase::IRBase(IR_TYPE type, GlobalVariable *val) : inst(nullptr), constant(nullptr), allo(nullptr), arg(nullptr),
                                                    global(nullptr), type(nullptr) {
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
    if (ir_type != IR_INST) {
        std::cout << "non-Inst IR Node interpreted as Inst IR Node!" << std::endl;
        exit(1);
    }
    return inst;
}

Alloca *IRBase::get_alloca() const {
    if (ir_type != IR_ALLOCA) {
        std::cout << "non-Alloca IR Node interpreted as Alloca IR Node!" << std::endl;
        exit(1);
    }
    return allo;
}

Constant *IRBase::get_constant() const {
    if (ir_type != IR_VALUE) {
        std::cout << "non-Value IR Node interpreted as Value IR Node!" << std::endl;
        exit(1);
    }
    return constant;
}

const std::string &IRBase::get_name() const {
    switch (this->ir_type) {
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
        case IR_GLOBAL_VAR:
            return global->get_name();
            break;
        case IR_ARG:
            return arg->get_name();
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
    switch (this->ir_type) {
        case IR_VALUE:
            return this->constant->get_value();
        case IR_INST:
            return this->inst->get_value();
        case IR_ALLOCA:
            assert(0);
            return this->allo->get_value();
        case IR_ARG:
            return this->arg->get_value();
        case IR_GLOBAL_VAR:
            return this->global->get_value();
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

FunctionType::FunctionType(Type *ret_ty) : ret(ret_ty) {
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


void Function::add_block(BasicBlock *b) {
    for (auto &bl: this->b_list)
        if (bl->get_name() == name) {
            std::cout << "BasicBlocks name repetition." << std::endl;
            exit(1);
        }
    this->b_list.insert(this->b_list.end(), std::unique_ptr<BasicBlock>(b));
}

Linkage Function::ExternalLinkage = ExternalLinkage;
Linkage Function::InternalLinkage = InternalLinkage;

Function::Function(FunctionType *ty, const std::string &_name, Linkage _link) :
        link(_link), v_reg_assigned(0), type(ty), name(_name) {
    for (auto &i: ty->get_params()) {
        arg_list.push_back(std::make_unique<Arg>(i));
    }
}

unsigned int &Function::get_reg() {
    return this->v_reg_assigned;
}

Alloca *Function::get_alloca(const std::string &name) {
    if (this->alloca_list.find(name) == this->alloca_list.end())
        return nullptr;
    else
        return this->alloca_list.find(name)->second;
}

void Function::add_alloca(Alloca *ptr) {
    auto name = ptr->get_name();
    if (this->alloca_list.find(name) != this->alloca_list.end()) {
        std::cout << "Internal Alloca name repetition" << std::endl;
        assert(0);
        exit(1);
    } else
        this->alloca_list[name] = ptr;
}

FunctionType *Function::get_func_type() const {
    return this->type;
}


const std::string &Arg::get_name() {
    return this->name;
}

void Arg::set_name(const std::string &n) {
    this->name = n;
}

Arg::Arg(Type *ty) {
    this->type = ty;
}

Type *Arg::get_type() const {
    return this->type;
}

std::string Arg::get_value() {
    return '%' + this->name;
}


BasicBlock::BasicBlock(const std::string &_name, Function *func) : name(_name), function(func) {
    insert_point = inst_list.end();
    this->range[0] = 0;
    this->range[1] = 0;
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

bool BasicBlock::isEmpty() const {
    return this->inst_list.empty();
}

Function *BasicBlock::get_func() {
    return this->function;
}

const unsigned int *BasicBlock::get_v_reg_range() const {
    return this->range;
}

void BasicBlock::set_v_reg_range(unsigned int v) {
    if (this->range[0] == this->range[1])
        this->range[0] = v;
    else
        this->range[1] = v;
}


std::list<std::unique_ptr<Alloca>> Alloca::alloca_list;

Alloca::Alloca(Type *ty, const std::string &_name, bool isConstant) : isConst(isConstant), initialized(false), type(ty),
                                                                      name(_name), v_reg(0) {}

Alloca *Alloca::Create(Type *ty, const std::string &name, bool isConstant) {
    auto res = new Alloca(ty, name, isConstant);
    Alloca::alloca_list.push_back(std::unique_ptr<Alloca>(res));
    return res;
}


Alloca::Alloca(Type *ty, const std::string &_name, IRBase *_val) : isConst(true), type(ty), name(_name), v_reg(0) {
    // complete
}

/**
 * seems only impossible to executable when an alloca's lifetime comes to an end.
 * @param ptr
 */
void Alloca::Kill(Alloca *ptr) {
    auto iter = Alloca::alloca_list.begin();
    for (; iter != Alloca::alloca_list.end(); iter++) {
        if (iter->get() == ptr) {
            Alloca::alloca_list.erase(iter);
            break;
        }
    }
}

Type *Alloca::get_type() const {
    return type;
}


Type *Alloca::get_element_type() const {
    return type->get_element_type();
}

const std::string &Alloca::get_name() const {
    return name;
}


std::string Alloca::get_value() {
    return '%' + this->name;
}

bool Alloca::isConstant() const {
    return this->isConst;
}

bool Alloca::isInitialized() const {
    return this->initialized;
}

void Alloca::Initialize() {
    this->initialized = true;
}

std::string Alloca::print_type() const {
    return Type::isPtr(this->type) ? this->type->print() + (this->type->isArrayType() ? "*" : "") :
           this->type->print() + '*';
}


// TODO: only consider type of int and bool
Constant *Constant::Create(ARITH_TYPE op, Constant *lhs, Constant *rhs) {
    int res = Type::isInt32(lhs->get_type()) ? lhs->value.int_val : lhs->value.bool_val;
    int operand = rhs != nullptr ? Type::isInt32(rhs->get_type()) ? rhs->value.int_val : rhs->value.bool_val : 0;
    bool resb;
    switch (op) {
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
            return Constant::get(Type::getInt1(), resb)->dyn_cast<Constant *>();
            break;
    }
    return Constant::get(Type::getInt32(), res)->dyn_cast<Constant *>();
}

Constant *Constant::Create(CMP_TYPE op, Constant *lhs, Constant *rhs) {
    int l = Type::isInt32(lhs->get_type()) ? lhs->value.int_val : lhs->value.bool_val;
    int operand = rhs != nullptr ? Type::isInt32(rhs->get_type()) ? rhs->value.int_val : rhs->value.bool_val : 0;
    bool res;
    switch (op) {
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
    return Constant::get(Type::getInt1(), res)->dyn_cast<Constant *>();
}

Constant *Constant::Create(LG_TYPE op, Constant *lhs, Constant *rhs) {
    auto l = Type::isInt32(lhs->get_type()) ? lhs->value.int_val : lhs->value.bool_val;
    auto operand = rhs != nullptr ? Type::isInt32(rhs->get_type()) ? rhs->value.int_val : rhs->value.bool_val : 0;
    bool res;
    switch (op) {
        case LG_AND:
            res = l && operand;
            break;
        case LG_OR:
            res = l || operand;
            break;
    }
    return Constant::get(Type::getInt1(), res)->dyn_cast<Constant *>();
}

std::string Constant::get_value() {
    switch (this->type->get_type()) {
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

GlobalVariable::GlobalVariable(Type *ty, bool constant, IRBase *_val, const std::string &ident) : type(ty),
                                                                                                  isConst(constant),
                                                                                                  initialized(true),
                                                                                                  val(_val),
                                                                                                  name(ident) {
    // complete
}

bool GlobalVariable::isInitialized() const {
    return this->initialized;
}

void GlobalVariable::Initialize() {
    assert(!this->initialized);
    this->initialized = true;
}

std::string GlobalVariable::get_value() {
    return '@' + this->name;
}

std::string GlobalVariable::get_initial_value() {
    return this->val->get_value();
}

std::string GlobalVariable::print_type() const {
    assert(0 && "GlobalVariable::print_type() undefined!");
    return "";
}


