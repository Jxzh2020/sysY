//
// Created by Hao Zhong on 5/18/23.
//
#include "IRGen/IRGen.h"
using namespace IRGen;

std::stringstream Module::print() {
    std::stringstream code;
    for( auto& func: func_list){
        code << func.second->print() << '\n';
    }
    return code;
}


std::string Type::print() const {
    switch(this->type){
        case INT32:
            return "i32";
        case INT1:
            return "i1";
        case VOID:
            return "void";
    }
}


std::string Arg::print_type() const {
    return this->type->print();
}

std::string Arg::print_name() const {
    return this->name;
}


std::string Function::arg_print_all() const {
    std::string code;
    code+='(';
    if(!this->arg_empty()){
        for( auto& i : this->arg_list){
            code+=(i->print_type()+" %"+i->print_name()+", ");
        }
        code = code.substr(0,code.length()-2);
    }
    code+=')';
    return code;
}

std::string Function::arg_print_type() const {
    std::string code;
    code+='(';
    if(!this->arg_empty()){
        for( auto& i : this->arg_list){
            code+=(i->print_type()+", ");
        }
        code = code.substr(0,code.length()-2);
    }
    code+=')';
    return code;
}


std::string Function::print_define() const {
    std::string code;
    code = "define " + this->type->get_ret_type()->print() + " @" + this->name + this->arg_print_all();
    return code;
}

std::string Function::print_declare() const {
    std::string code;
    code = "declare " + this->type->get_ret_type()->print() + " @" + this->name + this->arg_print_type();
    return code;
}

std::string Function::print() {
    std::stringstream code;
    // declare
    if(this->b_list.empty()){
        // should print : declare void @putch(i32)
        code << this->print_declare();
    }
    // define
    else{
        // should print : define i32 @test(i32 %a, i32 %b)
        code << this->print_define() << " {";
        for( auto& block : this->b_list){
            code << '\n' << block.first << ":\n" << block.second->print();
        }
        code << "}";
        return std::string(code.str());
    }
    return code.str();
}

std::string BasicBlock::print() {
    std::stringstream code;
    for( auto& inst: this->inst_list){
        code << "  " << inst->print() << '\n';
    }
    return code.str();
}

/* The following defs are all Inst print defs  */

std::string BranchInst::print() {
    std::string inst;
    Inst* cast;
    if(!this->isConBr){
        inst+=("br label %" + this->t_des->get_name());
        return inst;
    }
    else{
        // TODO: this could raise error, a pointer comparison here
//        cast = Type::Cast(this->con, Type::getInt1());
//        if(cast)
//            inst+=(cast->print() + "\n  ");
        inst+=(std::string("br i1 ") + this->con->get_value() + ", label %" + this->t_des->get_name() + ", label %" + this->f_des->get_name());
        return inst;
    }
}


std::string ArithInst::print() {

    static const std::string list[] = {"add", "sub", "???", "sdiv", "mul", "srem"};
    std::string opcode = list[this->op];
    std::string out;

    if(this->isConst){
        return out;
        //
        // return this->con_ptr->get_value();
    }
    else{
        // imcomplete!
        assert(this->lhs != nullptr && this->rhs != nullptr);
        out = (std::string("%")+std::to_string(this->v_reg)+" = "+opcode+" ");
        out+=(this->lhs->get_type()->print()+" ");
        out+=(this->lhs->get_value()+", "+this->rhs->get_value());
        return out;
    }
}


std::string AllocaInst::print() {
    std::string out;
    switch(this->op){
        case ALLOCA_CREATE:
            out = "; ALLOCA_CREATE";
            break;
        case ALLOCA_STORE:
            // val is evaluated
            assert(this->val);
            //this->ptr->set_value(this->val);
            out = "; ALLOCA_STORE";
            break;
        case ALLOCA_LOAD:
            out = "; ALLOCA_LOAD";
            //out += this->val->get_value();
            break;
    }
    return out;
}

std::string CmpInst::print() {
    static const std::string list[] = {"slt", "sle", "sgt", "sge", "ne", "eq"};
    std::string out;
    if( this->isConst)
        return out;
    out = "%" + std::to_string(this->v_reg) + " = icmp ";
    out+=list[this->op];
    out+=(" " + lhs->get_type()->print() + " " + lhs->get_value()+ ", " + rhs->get_value());

    return out;
}

std::string LogicInst::print() {
    static const std::string list[] = {""};
    std::string out;
    Inst* cast_l = nullptr, *cast_r = nullptr;

    if(true){//!Type::isInt1(this->lhs->get_type())){
        cast_l = this->lhs->dyn_cast<Inst*>() ? Type::Cast(this->lhs->dyn_cast<Inst*>(),Type::getInt1()) : Type::Cast(this->lhs->dyn_cast<Constant*>(), Type::getInt1());
        out+=(cast_l->print() + "\n  ");
    }
    if(true){//!Type::isInt1(this->rhs->get_type())){
        cast_r = this->rhs->dyn_cast<Inst*>() ? Type::Cast(this->rhs->dyn_cast<Inst*>(),Type::getInt1()) : Type::Cast(this->rhs->dyn_cast<Constant*>(), Type::getInt1());
        out+=(cast_r->print() + "\n  ");
    }

    out+=("%"+std::to_string(this->v_reg) + " = select i1 ");
    if(this->op == LG_OR){
        out+=(cast_l->get_value()+", i1 true, i1 "+cast_r->get_value());
    }
    else{
        out+=(cast_l->get_value()+", i1 "+cast_r->get_value()+", i1 false");
    }
    return out;
}

/**
 * Note that a Inst node may be a Constant
 */

/**
 *  Note: All insts are committed by builder,
 *  when printing, if inst is constant, there's no need
 *  to print the inst, just use it directly.
 *
 *  For latter insts, whenever a inst that is not Constant is referred,
 *  it's considered to be having been committed already.(manage by user)
 */
std::string RetInst::print() {
    std::string code("ret ");
    Inst* ins;
    Alloca* alloca;
    Constant* cons;
    if(!ret_val){
        code+="void";
        return code;
    }
    else{
        ins = this->ret_val->dyn_cast<Inst*>();
        cons = this->ret_val->dyn_cast<Constant*>();
        alloca = this->ret_val->dyn_cast<Alloca*>();

        if(ins)
            code+=(ins->get_type()->print()+" "+ins->get_value());
        else if(cons){
            code+=(cons->get_type()->print()+" "+cons->get_value());
        }
        else if(alloca){
            code+=(alloca->get_type()->print()+ " " +alloca->get_value());
        }
        else{
            std::cout << "RetInst::ret_val of unknown type!" << std::endl;
            exit(1);
        }
        return code;
    }
}
// TODO: unhandled RetInst::get_type()
Type *RetInst::get_type() const {
    //assert(false);
    return nullptr;
}

std::string RetInst::get_value() const {
    assert(false);
    return "";
}
