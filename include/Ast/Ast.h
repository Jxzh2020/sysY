//
// Created by Hao Zhong on 4/14/23.
//

#ifndef SYSY_AST_H
#define SYSY_AST_H

#include <iostream>
#include <memory>
#include <string>
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

// 所有 AST 的基类
class BaseAST{
public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
    [[nodiscard]] virtual llvm::Value* codegen() const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST{
public:
    // 用智能指针管理对象
    std::unique_ptr<BaseAST> func_def;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};

class FunctypeAST: public BaseAST{
public:
    std::string type;
    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};

class BlockAST: public BaseAST{
public:
    std::unique_ptr<BaseAST> stmt;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};

class StmtAST: public BaseAST{
public:
    std::unique_ptr<std::string> number;

    void Dump() const override;
    [[nodiscard]] llvm::Value* codegen() const override;
};



#endif //SYSY_AST_H
