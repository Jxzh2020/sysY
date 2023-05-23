//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_BASEAST_H
#define SYSY_BASEAST_H

#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <map>
#include <unordered_map>

#include "IRGen/IRGen.h"


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
    IRGen::BasicBlock *curblock;
    // to fully support variable shadowing, a local variable list is maintained
    std::unordered_map<IRGen::BasicBlock *, std::unique_ptr<std::unordered_map<std::string, IRGen::Alloca*>>> name_map;
    // logical first BB ---> logical block (or scope) alloca inst
    std::unordered_map<IRGen::BasicBlock *, std::vector<IRGen::Alloca *>> alloca;
    // logical block first BB ----> real LLVM BasicBlocks
    std::unordered_map<IRGen::BasicBlock *, std::vector<IRGen::BasicBlock *>> basic_blocks_of;
    // logical block ---> parent logical block first BB
    std::unordered_map<IRGen::BasicBlock *, IRGen::BasicBlock *> parent_block_of_logical;
    // real block ---> logical block first BB
    std::unordered_map<IRGen::BasicBlock *, IRGen::BasicBlock *> logical_block_of;
    //
    unsigned long long uid;

};

class BaseAST {
public:
    virtual ~BaseAST() = default;

    virtual void Dump() const = 0;

    [[nodiscard]] virtual IRGen::IRBase *codegen() = 0;
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

    std::unique_ptr<IRGen::Builder> &getBuilder() { return Builder; }

    std::unique_ptr<IRGen::Module> &getModule() { return TheModule; }

    std::map<std::string, IRGen::IRBase *> &getNameMap() { return NamedValues; }

    /**
     * Flush current function pointer and its logical context,
     * Do this manually when a new FuncDef is derived.
     *
     */
    void EnterFunc(IRGen::Function *cursor) {
        isglobe = false;
        if (func_list.find(cursor) == func_list.end()) { func_list[cursor] = std::make_unique<FuncContext>(); }
        curFunc = cursor;
        Func_Context = func_list[cursor].get();
        ClearBranch();
    }

    void ExitFunc() { isglobe = true; }

    bool isGlobeBlock() { return isglobe; }

    IRGen::Function *getFunc() { return curFunc; }

    void AddGlobe(IRGen::GlobalVariable *p) { global_variable.push_back(std::unique_ptr<IRGen::GlobalVariable>(p)); }

private:
    std::unique_ptr<IRGen::Builder> Builder;
    std::unique_ptr<IRGen::Module> TheModule;
    std::map<std::string, IRGen::IRBase *> NamedValues;
    IRGen::Function *curFunc;
    static std::unique_ptr<IR> instance;
    bool isglobe;
    std::vector<std::unique_ptr<IRGen::GlobalVariable>> global_variable;
    // [condition bb, false bb]
    std::stack<std::pair<IRGen::BasicBlock *, IRGen::BasicBlock *>> while_bb;
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
    bool isInWhile() { return !while_bb.empty(); }

    IRGen::BasicBlock *false_bb() { return while_bb.top().second; }

    IRGen::BasicBlock *condition_bb() { return while_bb.top().first; }

    // [condition bb, false bb]
    void push_while(std::pair<IRGen::BasicBlock *, IRGen::BasicBlock *> in) { while_bb.push(in); }

    void pop_while() { while_bb.pop(); }

    void EnterBlock(IRGen::BasicBlock *cursor) {
        Func_Context->curblock = cursor;
        Builder->SetInsertPoint(cursor);
    }

    IRGen::BasicBlock *currentBlock() { return Func_Context->curblock; }

    void setBlock(IRGen::BasicBlock *p) { Func_Context->curblock = p; }

//    inline IRGen::BasicBlock* getLBlock() { return logical_block_of[curblock]; }
//    inline IRGen::BasicBlock* getFBlock() { return parent_block_of_logical[logical_block_of[curblock]]; }
//    std::vector<IRGen::AllocaInst*>& getAlloca() { return alloca[logical_block_of[curblock]]; }
    //void RetParent();

private:
    std::unordered_map<IRGen::Function *, std::unique_ptr<FuncContext>> func_list;
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
    std::string CreateName() { return curFunc->get_name() + std::to_string(Func_Context->uid++); }

    void NewLogicalBlockStart();

    void NewLogicalBlockEnd();

    IRGen::BasicBlock *NewBasicBlock();

    void AddAlloca(IRGen::IRBase *, const std::string &);

    IRGen::IRBase *GetAlloca(const std::string &);

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

    IRGen::IRBase *GetAlloca(const std::string &, IRGen::BasicBlock *);

};


#endif //SYSY_BASEAST_H
