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

enum UnaryOp {
    PLUS,
    MINUS,
    COMPLEMENT
};

class BaseAST{
public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
    [[nodiscard]] virtual llvm::Value* codegen() const = 0;
};

/*
 * class IR maintains essential LLVM classes to create an in-memory form IR in one module.
 * To use LLVM context, module, builder or namevalues, simply IR::get()->getXXX()
 *
 * */
class IR{
public:
    IR();
    static std::unique_ptr<IR>& get();
    std::unique_ptr<llvm::LLVMContext>& getContext() { return TheContext; }
    std::unique_ptr<llvm::IRBuilder<>>& getBuilder() { return Builder; }
    std::unique_ptr<llvm::Module>& getModule() { return TheModule; }
    std::map<std::string, llvm::Value *>& getNameMap() { return NamedValues; }
    void EnterFunc(llvm::Function* cursor) { curFunc = cursor; }
    llvm::Function* GetFunc() { return curFunc; }
private:
    std::unique_ptr<llvm::LLVMContext> TheContext;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::unique_ptr<llvm::Module> TheModule;
    std::map<std::string, llvm::Value *> NamedValues;
    llvm::Function* curFunc;
    static std::unique_ptr<IR> instance;
};


#endif //SYSY_BASEAST_H
