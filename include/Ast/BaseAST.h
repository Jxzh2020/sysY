//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_BASEAST_H
#define SYSY_BASEAST_H

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/Instructions.h>


enum UnaryOp {
    PLUS,
    MINUS,
    COMPLEMENT
};
enum BinaryOp {
    MUL,
    DIV,
    MOD
};
enum LogicOp {
    OR,
    AND,
};
enum CompOp {
    LT,
    GT,
    LE,
    GE,
    EQ,
    NEQ
};

/**
 * Some Contexts essential for logical and basic block inside a function,
 * which maintains the logical structure of a function
 */
struct FuncContext {
    FuncContext() : curblock(nullptr) {}

    // current BB
    llvm::BasicBlock *curblock;
    // to fully support variable shadowing, a local variable list is maintained
    std::unordered_map<llvm::BasicBlock *, std::unique_ptr<std::unordered_map<std::string, llvm::AllocaInst *>>> name_map;
    // logical first BB ---> logical block (or scope) alloca inst
    std::unordered_map<llvm::BasicBlock *, std::vector<llvm::AllocaInst *>> alloca;
    // logical block first BB ----> real LLVM BasicBlocks
    std::unordered_map<llvm::BasicBlock *, std::vector<llvm::BasicBlock *>> basic_blocks_of;
    // logical block ---> parent logical block first BB
    std::unordered_map<llvm::BasicBlock *, llvm::BasicBlock *> parent_block_of_logical;
    // real block ---> logical block first BB
    std::unordered_map<llvm::BasicBlock *, llvm::BasicBlock *> logical_block_of;
    //
    unsigned long long uid;

};

class BaseAST {
public:
    virtual ~BaseAST() = default;

    virtual void Dump() const = 0;

    [[nodiscard]] virtual llvm::Value *codegen() const = 0;
};

/*
 * class IR maintains essential LLVM classes to create an in-memory form IR in one module.
 * To use LLVM context, module, builder or name_values, simply IR::get()->getXXX()
 *
 * */
class IR {
public:
    IR();

    static std::unique_ptr<IR> &get();

    std::unique_ptr<llvm::LLVMContext> &getContext() { return TheContext; }

    std::unique_ptr<llvm::IRBuilder<>> &getBuilder() { return Builder; }

    std::unique_ptr<llvm::Module> &getModule() { return TheModule; }

    std::map<std::string, llvm::Value *> &getNameMap() { return NamedValues; }

    /**
     * Flush current function pointer and its logical context,
     * Do this manually when a new FuncDef is derived.
     *
     */
    void EnterFunc(llvm::Function *cursor) {
        isglobe = false;
        if (func_list.find(cursor) == func_list.end()) { func_list[cursor] = std::make_unique<FuncContext>(); }
        curFunc = cursor;
        Func_Context = func_list[cursor].get();
    }

    void ExitFunc() { isglobe = true; }

    bool isGlobeBlock() { return isglobe; }

    llvm::Function *getFunc() { return curFunc; }

    void AddGlobe(llvm::GlobalVariable *p) { global_variable.push_back(std::unique_ptr<llvm::GlobalVariable>(p)); }

private:
    std::unique_ptr<llvm::LLVMContext> TheContext;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::unique_ptr<llvm::Module> TheModule;
    std::map<std::string, llvm::Value *> NamedValues;
    llvm::Function *curFunc;
    static std::unique_ptr<IR> instance;
    bool isglobe;
    std::vector<std::unique_ptr<llvm::GlobalVariable>> global_variable;


public:
    void EnterBlock(llvm::BasicBlock *cursor) {
        Func_Context->curblock = cursor;
        Builder->SetInsertPoint(cursor);
    }

    llvm::BasicBlock *currentBlock() { return Func_Context->curblock; }

    void setBlock(llvm::BasicBlock *p) { Func_Context->curblock = p; }

//    inline llvm::BasicBlock* getLBlock() { return logical_block_of[curblock]; }
//    inline llvm::BasicBlock* getFBlock() { return parent_block_of_logical[logical_block_of[curblock]]; }
//    std::vector<llvm::AllocaInst*>& getAlloca() { return alloca[logical_block_of[curblock]]; }
    //void RetParent();

private:
    std::unordered_map<llvm::Function *, std::unique_ptr<FuncContext>> func_list;
    FuncContext *Func_Context;


public:
    /**
     * automatically manage logical blocks, users only care about basic blocks
     *
     * Usage:
     * NewLogicalBlockStart();  // auto enter a new basic block
     * block->codegen();
     * NewLogicalBlockEnd();    // auto exit new logical block, release alloca and jump back to
     *                          // current logical block's new basic block
     *
     *
     */
    std::string CreateName() { return curFunc->getName().str() + std::to_string(Func_Context->uid++); }

    void NewLogicalBlockStart();

    void NewLogicalBlockEnd();

    llvm::BasicBlock *NewBasicBlock();

    void AddAlloca(llvm::AllocaInst *, const std::string &);

    llvm::Value *GetAlloca(const std::string &);

private:
    llvm::Value *GetAlloca(const std::string &, llvm::BasicBlock *);

};


#endif //SYSY_BASEAST_H
