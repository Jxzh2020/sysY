//
// Created by Hao Zhong on 5/15/23.
//

#ifndef SYSY_IRGEN_H
#define SYSY_IRGEN_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
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
    class GlobalVariable;
    class Alloca;

    /**
     *  For v reg and operation mapping
     */
    class Inst;

    /** The namespace Linkage is only set to follow LLVM code style.
     *
     */

    enum Linkage {
        ExternalLinkage,
        InternalLinkage
    };

    enum IR_TYPE {
        IR_INST,
        IR_VALUE
    };
    enum P_TYPE {
        INT32,
        INT1,
        VOID
    };

    /**
     *  ???
     */
    enum INST_TYPE {
        ADD,
        SUB,
        NEG
    };


    class Module {
    public:
        [[nodiscard]] IRBase* getGlobalVariable(const std::string& name) const;
    private:
        std::unordered_map<std::string,std::unique_ptr<Function> > func_list;
        std::unordered_map<std::string, std::unique_ptr<GlobalVariable> > global_var_list;
    };

    /**
     *  when calling Create function, class Inst is wrapped in the IRBase* return value.
     *  By user calling Create functions to connect an actual flow, inside IRGen, a tree of Inst is created, at this moment,
     *  any actual implementation details is not considered.
     *  Only when calling Module::print, does IRGen start to cope with llvm ir related details, like virtual register management, lift time management and so on.
     */
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

        IRBase* CreateGlobalVariable(Module* _module, Type* ty, bool isConstant, Linkage linkage, IRBase* Initializer, const std::string& name);

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

        IRBase* CreateLifetimeStart( Alloca* );
        IRBase* CreateLifetimeEnd( Alloca*);

    private:
        BasicBlock* current_at_bb;

    };

    /** Type class
     *  cope with all the primitive types and complex type
     *  Note: this is gonna be a complicated class if well-defined
     */
    class Type {
    public:
        static Type* getInt32();
        static Type* getInt1();
//        static Type getInt32();
        static Type* getVoid();
//        static Type getInt32();
        // need modification when add self-defined type
        P_TYPE get_type();
    private:
        explicit Type(P_TYPE);
        static void gen_all_instances();
        static std::vector<std::unique_ptr<Type> > allocated;
        P_TYPE type;
    };

    /** IRBase should be a basic interface for all values inst and create inst
     *  By IRBase::inst, IRBase* in Builder->Create__( __, __) can be converted to insts
     *
     *  In order to implement a simple and consistent interface for Builder::Create__,
     *  inst, reference and alloca is wrapped up to IRBase
     *
     */
    class IRBase {
    public:
        [[nodiscard]] virtual const std::string& get_name() const { return this->name; }
        [[nodiscard]] virtual Type* get_type() { return this->type; }


        static IRBase* CreateIRBase(IR_TYPE type, Inst* val);
        static IRBase* CreateIRBase(IR_TYPE type, Constant* val);
    protected:
        std::string name;
        Type* type;
    private:
        explicit IRBase(Inst* val);

        explicit IRBase(Constant* val);

        static std::vector<std::unique_ptr<IRBase> > base_list;
        Inst* inst;
        Constant* constant;
        IR_TYPE ir_type;
    };

    /**
     *  Constant should provide a well-defined value-type system, including type cast, multiple type support and so on.
     */

    class Constant {
    public:
        template<class T>
        static IRBase* get(Type* ty, T val);
        Type* get_type();
    private:
        explicit Constant(Type* ty);
        static std::vector<std::unique_ptr<Constant> > const_list;
        Type* type;
        union {
            int int_val;
            bool bool_val;
            // more to come
        } value;
    };




    class FunctionType {
    public:
        static FunctionType* get(Type* ret_ty, std::vector<Type*> params, bool isVarArg);

    };

    /**
     * Real class that store all the IR instructions
     */
    class BasicBlock {
    public:
        static BasicBlock* Create( const std::string& name, Function* function);

        const std::string& get_name() const;
        void set_name(const std::string& name);
    private:
        std::list<std::unique_ptr<Inst*> > inst_list;
        std::list<std::unique_ptr<Inst*> >::iterator insert_point;
    };




    class Inst {
    public:

    private:
        unsigned int v_reg;
    };


    class Function: public IRBase {
    public:
        static Function* Create(FunctionType* ty, const std::string& name, Module* module);

        Type* get_type() override;
        const std::string& get_name() const override;

        bool arg_empty() const;
        std::vector<std::unique_ptr<IRBase*> >::iterator arg_begin();
        std::vector<std::unique_ptr<IRBase*> >::iterator arg_end();
    private:
        class Arg: public IRBase {
            ;
        };
        /**
         * TODO: Remember to define these two static member
         */
    public:
        static Linkage ExternalLinkage;
        static Linkage InternalLinkage;
    private:
        std::vector<std::unique_ptr<IRBase> > arg_list;
        std::vector<std::unique_ptr<BasicBlock> > b_list;
        unsigned int v_reg;

    };
    class GlobalVariable: public IRBase {
    public:
        Type* get_type() override;
        const std::string& get_name() const override;
        bool isConstant() const;
    private:
        bool isConst;
    };
    class Alloca: public IRBase {
    public:
        Type* get_type() override;
        const std::string& get_name() const override;
    };




};


#endif //SYSY_IRGEN_H
