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


std::string Function::Arg::print_type() const {
    return this->type->print();
}

std::string Function::Arg::print_name() const {
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
            code << '\n' << block.first << ":\n" << block.second->print(this->v_reg_assigned);
        }
        code << "}";
        return std::string(code.str());
    }
    return code.str();
}

std::string BasicBlock::print(unsigned int &st) {
    std::stringstream code;
    for( auto& inst: this->inst_list){
        code << "  " << inst->print(st) << '\n';
    }
    return code.str();
}

/* The following defs are all Inst print defs  */

std::string BranchInst::print(unsigned int &st) {
    std::string inst;
    Inst* cast;
    if(!this->isConBr){
        inst+=("br label %" + this->t_des->get_name());
        return inst;
    }
    else{
        // TODO: this could raise error, a pointer comparison here
        cast = Type::Cast(this->con, Type::getInt1());
        inst+=(cast->print(st)+"\n  ");
        inst+=(std::string("br i1 %") + std::to_string(st++) + ", label %" + this->t_des->get_name() + ", label %" + this->f_des->get_name());
        return inst;
    }
}


std::string ArithInst::print(unsigned int &st) {
    return std::string();
}

std::string AllocaInst::print(unsigned int &st) {
    return std::string();
}

std::string CmpInst::print(unsigned int &st) {
    return std::string();
}

std::string LogicInst::print(unsigned int &st) {
    return std::string();
}

std::string RetInst::print(unsigned int &st) {
    std::string code("ret ");
    Inst* ins;
    if(!ret_val){
        code+="void";
        return code;
    }
    else{
        ins = this->ret_val->dyn_cast<Inst*>();
        if(ins)
            code+=(ins->get_type()->print()+" %"); //+std::to_string(ins->getVReg()));
        return code;
    }
}