//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_BASEAST_H
#define SYSY_BASEAST_H

#include <iostream>
#include <sstream>
#include <memory>
#include <stack>
#include <string>
#include <map>
#include <unordered_map>
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
    FuncContext() : curblock(nullptr), uid(0) {}

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

    virtual std::string astJson(int size) = 0;

    [[nodiscard]] virtual llvm::Value *codegen() = 0;
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
        ClearBranch();
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
    // [condition bb, false bb]
    std::stack<std::pair<llvm::BasicBlock*,llvm::BasicBlock*>> while_bb;
    bool hasBranch;

/** To support break and continue clause, some inf should remain during Stmts parsing.
 *
 *
 * */
public:
    bool hasBranchAtEnd() const { return hasBranch; }
    void SetBranch() { hasBranch = true; }
    void ClearBranch() { hasBranch = false; }


public:
    bool isInWhile() { return !while_bb.empty() ;}
    llvm::BasicBlock* false_bb() { return while_bb.top().second; }
    llvm::BasicBlock* condition_bb() { return while_bb.top().first; }
    // [condition bb, false bb]
    void push_while(std::pair<llvm::BasicBlock*, llvm::BasicBlock*> in) { while_bb.push(in); }
    void pop_while() { while_bb.pop(); }
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
    /** declare lib functions
     *  int getint();：输入一个整数，返回对应的整数值。
     *  int getch();：输入一个字符，返回字符对应的 ASCII 码值。
     *  void putint(int);：输出一个整数的值。
     *  void putch(int);：输出一个 ASCII 码对应的字符。传入的整数参数取值范围为 0~255。
     *
     *  int getarray(int []);：输入一串整数，第 1 个整数代表后续要输入的整数个数，该个数通过返回值返回；后续的整数通过传入的数组参数返回。getarray() 不会检查调用者提供的数组是否有足够的空间容纳输入的一串整数。
     *  void putarray(int, int[]); 第 1 个参数表示要输出的整数个数（假设为 N），后面应该跟上要输出的 N 个整数的数组。putarray() 在输出时会在整数之间安插空格。
     *
     */
    void declare_libfunc();
    llvm::Value *GetAlloca(const std::string &, llvm::BasicBlock *);

};

std::string Escape(std::string input);

// string Json_no_child(string name);

// string Json_with_children(string name, std::vector<std::string> children);

std::string Json(std::string name, std::vector<std::string> children, int size);
std::string Json(std::string name, int size);
int sizeplus(int size);

#endif //SYSY_BASEAST_H
