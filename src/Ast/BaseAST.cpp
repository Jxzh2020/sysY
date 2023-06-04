//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/BaseAST.h"
#include "IRGen/IRGen.h"

std::vector<std::unique_ptr<IRGen::Type>> IRGen::Type::allocated;
std::vector<std::unique_ptr<IRGen::Type>> IRGen::Type::compound;

#include <string>

std::unique_ptr<IR> IR::instance(new IR);

std::unique_ptr<IR> &IR::get() { return instance; }

IR::IR() {
    // Open a new context and module.
    TheModule = std::make_unique<IRGen::Module>("jxzh_sysY");

    // Create a new builder for the module.
    Builder = std::make_unique<IRGen::Builder>();
    curFunc = nullptr;
    Func_Context = nullptr;
    isglobe = true;
    hasBranch = false;

    declare_libfunc();
}

// void IR::RetParent() {
//     // release all the alloca of current logical block;
//     for(auto i: alloca[logical_block_of[curblock]]){
//         Builder->CreateLifetimeEnd(i);
//     }
//
//     // branch to the end of the parent logical block
//     // Create the BB of the parent logical block
//     auto block =
//     IRGen::BasicBlock::Create(TheContext,this->CreateName(),curFunc);
//     basic_blocks_of[parent_block_of_logical[logical_block_of[curblock]]].push_back(block);
//     logical_block_of[block] =
//     parent_block_of_logical[logical_block_of[curblock]];
//
//     // jump back to parent block
//     Builder->CreateBr(block);
//     Builder->SetInsertPoint(block);
//     // switch to parent block
//     curblock = block;
// }

void IR::NewLogicalBlockStart() {
    auto l_b = IRGen::BasicBlock::Create(this->CreateName(), curFunc);
    // TODO wired usage
    Func_Context->basic_blocks_of[l_b].push_back(l_b);
    Func_Context->logical_block_of[l_b] = l_b;

    // if is the definition block
    if (Func_Context->curblock == nullptr)
        Func_Context->parent_block_of_logical[l_b] = nullptr;
        // a nested block
    else {
        Func_Context->parent_block_of_logical[l_b] =
                Func_Context->logical_block_of[Func_Context->curblock];
        Builder->CreateBr(l_b);
    }
    Builder->SetInsertPoint(l_b);
    Func_Context->curblock = l_b;
}

void IR::NewLogicalBlockEnd() {
    // if at the end of block is a branch, do nothing
    if (hasBranch)
        return;
    // if is the definition block, no need to return to parent, and a RET inst is
    // done manually.
    if (Func_Context->parent_block_of_logical
        [Func_Context->logical_block_of[Func_Context->curblock]] == nullptr)
        return;
    for (auto i:
            Func_Context
                    ->alloca[Func_Context->logical_block_of[Func_Context->curblock]]) {
        // Builder->CreateLifetimeEnd(i);
    }

    auto block = IRGen::BasicBlock::Create(this->CreateName(), curFunc);
    Func_Context
            ->basic_blocks_of
    [Func_Context->parent_block_of_logical
    [Func_Context->logical_block_of[Func_Context->curblock]]]
            .push_back(block);
    Func_Context->logical_block_of[block] =
            Func_Context->parent_block_of_logical
            [Func_Context->logical_block_of[Func_Context->curblock]];

    // jump back to parent block
    Builder->CreateBr(block);
    Builder->SetInsertPoint(block);
    // switch to parent block
    Func_Context->curblock = block;
}

IRGen::BasicBlock *IR::NewBasicBlock() {
    auto bb = IRGen::BasicBlock::Create(CreateName(), curFunc);
    Func_Context
            ->basic_blocks_of[Func_Context->logical_block_of[Func_Context->curblock]]
            .push_back(bb);
    Func_Context->logical_block_of[bb] =
            Func_Context->logical_block_of[Func_Context->curblock];
    return bb;
}

void IR::AddAlloca(IRGen::IRBase *_al, const std::string &o_name) {
    auto al = dynamic_cast<IRGen::AllocaInst *>(_al->dyn_cast<IRGen::Inst *>())
            ->get_alloca();
    /****    Add to Alloca list   ****/
    Func_Context->alloca[Func_Context->logical_block_of[Func_Context->curblock]]
            .push_back(al);

    /****    Construct Name mapping to support shadowing    ****/
    if (Func_Context->name_map.find(
            Func_Context->logical_block_of[Func_Context->curblock]) ==
        Func_Context->name_map.end()) {
        Func_Context
                ->name_map[Func_Context->logical_block_of[Func_Context->curblock]] =
                std::make_unique<std::unordered_map<std::string, IRGen::Alloca *>>();
    }
    auto &map =
            Func_Context
                    ->name_map[Func_Context->logical_block_of[Func_Context->curblock]];
    if (map->find(o_name) != map->end()) {

        std::cout << "Error! Repeated variable name in one block!" << std::endl;
        assert(0);
    }
    (*map)[o_name] = al;

    /****    Set Variable LifeTime    ****/
    // top block
    if (Func_Context->parent_block_of_logical
        [Func_Context->logical_block_of[Func_Context->curblock]] == nullptr)
        return;
    // Builder->CreateLifetimeStart(al);
}

IRGen::IRBase *IR::GetAlloca(const std::string &name) {
    if (!Func_Context) {
        std::cout << "Fetching Allocas Not in Function Context" << std::endl;
        assert(0);
        return nullptr;
    }
    return GetAlloca(name, Func_Context->curblock);
}

IRGen::IRBase *IR::GetAlloca(const std::string &name, IRGen::BasicBlock *cur) {
    if (cur == nullptr) {
        return nullptr;
    }
    if (Func_Context->name_map.find(Func_Context->logical_block_of[cur]) ==
        Func_Context->name_map.end()) {
        return GetAlloca(
                name,
                Func_Context
                        ->parent_block_of_logical[Func_Context->logical_block_of[cur]]);
    }
    auto &map = Func_Context->name_map[Func_Context->logical_block_of[cur]];
    if (map.get()->find(name) == map->end()) {
        return GetAlloca(
                name,
                Func_Context
                        ->parent_block_of_logical[Func_Context->logical_block_of[cur]]);
    } else
        return IRGen::IRBase::CreateIRBase(IRGen::IR_INST, (*map)[name]);
}

void IR::declare_libfunc() {
    auto int_type = IRGen::Type::getInt32();
    auto void_type = IRGen::Type::getVoid();

    IRGen::Function::Create(IRGen::FunctionType::get(int_type),
                            IRGen::Function::ExternalLinkage, "getint",
                            TheModule.get());
    IRGen::Function::Create(IRGen::FunctionType::get(int_type),
                            IRGen::Function::ExternalLinkage, "getch",
                            TheModule.get());
    IRGen::Function::Create(IRGen::FunctionType::get(void_type, {int_type}),
                            IRGen::Function::ExternalLinkage, "putint",
                            TheModule.get());
    IRGen::Function::Create(IRGen::FunctionType::get(void_type, {int_type}),
                            IRGen::Function::ExternalLinkage, "putch",
                            TheModule.get());
}

//去除转义字符
std::string Escape(std::string input) {
    // s is our escaped output string
    std::string ret = "\"";
    std::stringstream stream;
    // loop through all characters
    for (char c: input) {
        // check if a given character is printable
        // the cast is necessary to avoid undefined behaviour
        if (c == '\"' || c == '\'' || c == '\\')
            ret = ret + "\\" + c;
        else if (c == '\n')
            ret += "\\n";
        else if (c == '\t')
            ret += "\\t";
        else if (c == '\r')
            ret += "\\r";
        else if (isprint((unsigned char) c))
            ret += c;
        else {
            stream << std::hex << (unsigned int) (unsigned char) (c);
            std::string code = stream.str();
            ret +=
                    std::string("\\x") + (code.size() < 2 ? "0" : "") + code;

        }

    }
    return ret + "\"";
}

std::string Json(std::string name, std::vector<std::string> children,
                 int size) {
    std::string ret = "{ \"name\" : " + Escape(name);
    if (children.empty() || children.size() == 0) {
        ret = ret + ", \"size\":" + std::to_string(size) + " }";
    } else {
        ret += ", \"children\" : [ ";
        int i;
        for (i = 0; i < children.size() - 1; i++) {
            ret +=  children[i] + ", ";
        }
        ret = ret +  children[i] + "], \"size\":" + std::to_string(size) + "}";
    }
    return ret;
}

std::string Json(std::string name, std::string child, int size)
{
    std::string ret = "{ \"name\" : " + Escape(name);
    ret += ", \"children\" : [ ";
    ret = ret + child + "], \"size\":" + std::to_string(size) + "}";

    return ret;
}

std::string Json(std::string name, int size)
{
    std::string ret = "{ \"name\" : " + Escape(name);
    ret = ret + ", \"size\":" + std::to_string(size) + " }";
    return ret;
}

int sizeplus(int size)
{
    if (size >+500)
        return size - 100;
    else
        return size;        
}