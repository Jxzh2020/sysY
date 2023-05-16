//
// Created by Hao Zhong on 5/15/23.
//

#ifndef SYSY_IRGEN_H
#define SYSY_IRGEN_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <list>


/** IRGen namespace:
 *  IRGen namespace implements the part of LLVM Lib used in sysY project
 *  Within the namespace, memory recycle is implemented to avoid leakage.
 */
namespace IRGen {

    // declare classes
    class Module;
    class Builder;
    class IRBase;
    class Type;
    class Constant;
    class BasicBlock;
    class AllocaInst;
    class FunctionType;
    class Function;


    enum IR_TYPE {
        IR_INT,
        IR_VOID
    };
    class Module {
    public:
        IRBase* getGlobalVariable(const std::string& name) const;
    private:
        std::unordered_map<std::string,Function*> func_list;
    };

    class Builder {
    public:
        IRBase* CreateBr( BasicBlock* Des);
        IRBase* CreateConBr( IRBase* condition, BasicBlock* true_b, BasicBlock* false_b);

        IRBase* CreateAdd( IRBase* LHS, IRBase* RHS);
        IRBase* CreateSub( IRBase* LHS, IRBase* RHS);
        IRBase* CreateNeg( IRBase* LHS, IRBase* RHS);

        IRBase* CreateMul( IRBase* LHS, IRBase* RHS);
        IRBase* CreateSDiv( IRBase* LHS, IRBase* RHS);
        IRBase* CreateSRem( IRBase* LHS, IRBase* RHS);

        Alloca* CreateAlloca( Type* ty, const std::string& name);
        Alloca* CreateStore( IRBase* val, Alloca* ptr);
        Alloca* CreateLoad( Alloca* ptr, IRBase* reg);

        GlobalVariable* CreateGlobalVariable(Module* _module, Type* ty, bool isConstant, Linkage linkage, IRBase* Initializer, const std::string& name);

        IRBase* CreateICmpSLT( IRBase* LHS, IRBase* RHS);
        IRBase* CreateICmpSGT( IRBase* LHS, IRBase* RHS);
        IRBase* CreateICmpSLE( IRBase* LHS, IRBase* RHS);
        IRBase* CreateICmpSGE( IRBase* LHS, IRBase* RHS);
        IRBase* CreateICmpEQ( IRBase* LHS, IRBase* RHS);
        IRBase* CreateICmpNE( IRBase* LHS, IRBase* RHS);

        IRBase* CreateLogicalOr( IRBase* LHS, IRBase* RHS);
        IRBase* CreateLogicalAnd( IRBase* LHS, IRBase* RHS);

        IRBase* CreateRetVoid();
        IRBase* CreateRet( IRBase* val);

        IRBase* CreateCall( Function* fun);
        IRBase* CreateCall( Function* fun , std::vector<IRBase*> );

        BasicBlock* SetInsertPoint( BasicBlock* basicblock);

    };

    /** Type class
     *  cope with all the primitive types and complex type
     */
    class Type {
    public:
        static Type getInt32();
        static Type getInt1();
//        static Type getInt32();
        static Type getVoid();
//        static Type getInt32();
    private:
        ;
    };
    class Constant {
        template<class T>
        static IRBase* get(Type* ty, T val);
    };


    class FunctionType {
    public:
        static FunctionType* get(Type* ret_ty, std::vector<Type*> params, bool isVarArg);
    };

    class BasicBlock {
    public:
        static BasicBlock* Create( const std::string& name, Function* function);

        const std::string& get_name() const;
        void set_name(const std::string& name);
    };


    /**
     *
     */

    class Function: public IRBase {
    public:
        static Function* Create(FunctionType* ty, const std::string& name, Module* module);

        Type* get_type() const;
        const std::string& get_name() const override;

        bool arg_empty() const;
        ArgIter& arg_begin();
        ArgIter& arg_end();
    private:
        class Arg: public IRBase {

        };
        class ArgIter {
            ;
        };
    public:

    };



    class IRBase {
    public:
        [[nodiscard]] const std::string& get_name() const { return this->name; }
        [[nodiscard]] const Type& get_type() const { return this->type; }
    private:
        std::string name;
        Type type;
    };
};


#endif //SYSY_IRGEN_H
