//
// Created by Hao Zhong on 5/15/23.
//

#ifndef SYSY_IRGEN_H
#define SYSY_IRGEN_H

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <memory>


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
        IR_VALUE,
        IR_ALLOCA
    };
    enum P_TYPE {
        INT32,
        INT1,
        VOID
    };

    /**
     *  ???
     */
    enum ARITH_TYPE {
        IR_ADD,
        IR_SUB,
        IR_NEG,
        IR_MUL,
        IR_S_DIV,
        IR_S_REM
    };

    enum ALLOCA_TYPE {
        ALLOCA_CREATE,
        ALLOCA_STORE,
        ALLOCA_LOAD
    };

    enum CMP_TYPE {
        CMP_LT,
        CMP_LE,
        CMP_GT,
        CMP_GE,
        CMP_NE,
        CMP_EQ
    };

    enum LG_TYPE {
        LG_AND,
        LG_OR
    };

    /**
     *  The internal type cast function.
     */



    class Module {
    public:
        [[nodiscard]] IRBase* getGlobalVariable(const std::string& name) const;
        void add_func(const std::string&, Function*);
        std::stringstream print();
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
        Builder();
        IRBase* CreateBr( BasicBlock* Des);
        IRBase* CreateConBr( IRBase* condition, BasicBlock* true_b, BasicBlock* false_b);

        IRBase* CreateAdd( IRBase* LHS, IRBase* RHS);
        IRBase* CreateSub( IRBase* LHS, IRBase* RHS);
        IRBase* CreateNeg( IRBase* LHS);

        IRBase* CreateMul( IRBase* LHS, IRBase* RHS);
        IRBase* CreateSDiv( IRBase* LHS, IRBase* RHS);
        IRBase* CreateSRem( IRBase* LHS, IRBase* RHS);

        /**
         *  kind of complicated here, builder has to check name repetition,
         *  if there is, rename the alloca.
         */
        IRBase * CreateAlloca( Type* ty, const std::string& name);
        IRBase* CreateStore( IRBase* val, IRBase* ptr);
        IRBase* CreateLoad( IRBase* ptr, IRBase* reg);

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
        IRBase* CreateCall( Function* fun , std::vector<IRBase*> args);

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
        static bool isInt32(Type* ty);
        static bool isInt1(Type* ty);
        static bool isVoid(Type* ty);
        // need modification when add self-defined type
        P_TYPE get_type();
        std::string print() const;
        /**
         *
         * @param from_inst the value to cast from
         * @param to_type the type cast to
         * @return cast instruction
         */
        static Inst* Cast(Constant* from, Type* to_type);
        static Inst* Cast(Inst* from_inst, Type* to_type);
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
        [[nodiscard]] const std::string& get_name() const;
        [[nodiscard]] Type* get_type();


        static IRBase* CreateIRBase(IR_TYPE type, Inst* val);
        static IRBase* CreateIRBase(IR_TYPE type, Constant* val);
        static IRBase* CreateIRBase(IR_TYPE type, Alloca* val);

        template <class U>
        U dyn_cast();

    private:
        [[nodiscard]] Inst* get_inst() const;
        [[nodiscard]] Alloca* get_alloca() const;
        [[nodiscard]] Constant* get_constant() const;

        explicit IRBase(IR_TYPE type, Inst* val);

        explicit IRBase(IR_TYPE type, Constant* val);

        explicit  IRBase(IR_TYPE type, Alloca* val);

        static std::vector<std::unique_ptr<IRBase> > base_list;

        Inst* inst;
        Constant* constant;
        Alloca* alloca;

        Type* type;
        IR_TYPE ir_type;
    };

    /**
     *  Constant should provide a well-defined value-type system, including type cast, multiple type support and so on.
     */

    class Constant {
    public:
        template<class T>
        static IRBase* get(Type* ty, T val);

        static Constant* Create(ARITH_TYPE op, Constant* lhs, Constant* rhs);
        static Constant* Create(CMP_TYPE op, Constant* lhs, Constant* rhs);
        static Constant* Create(LG_TYPE op, Constant* lhs, Constant* rhs);

        Type* get_type() const;
        const std::string& get_name() const;
    private:
        explicit Constant(Type* ty);
        static std::vector<std::unique_ptr<Constant> > const_list;
        Type* type;
        /**
         *  Usually name is empty, except const variable
         */
        std::string name;
        union {
            int int_val;
            bool bool_val;
            // more to come
        } value;
    };


    class FunctionType {
    public:
        static FunctionType* get(Type* ret_ty, std::vector<Type*>& params, bool isVarArg);
        std::vector<Type*>& get_params();
        Type* get_ret_type();
    private:
        FunctionType(Type* ret_ty, std::vector<Type*>& params, bool isVarArg);
        static std::vector<std::unique_ptr<FunctionType>> list;
        bool isVarArg;
        std::vector<Type*> params;
        Type* ret;
    };

    /**
     * Real class that store all the IR instructions,
     * Function takes control of BasicBlock memory management
     *
     * Not sure if same name block in different function work fine,
     * So now it's ok to arrange two same-name basic block into different function
     *
     *
     * basicblock doesnot repeat, there's no need to keep evaluated.
     *
     */
    class BasicBlock {
    public:
        static BasicBlock* Create( const std::string& name, Function* function);
        std::string print(unsigned int& st);
        [[nodiscard]] const std::string& get_name() const;
        void insert(Inst* );
    private:
        explicit BasicBlock(const std::string& name);
        // TODO: not sure if Inst memory should be managed by BasicBlock
        std::list<Inst* > inst_list;
        std::list<Inst* >::iterator insert_point;
        std::string name;
    };


    class Function {
    private:
        explicit Function(FunctionType* ty, const std::string& name);
        class Arg {
        public:
            explicit Arg(Type*);
            const std::string& get_name();
            std::string print_type() const;
            std::string print_name() const;
            void set_name(const std::string&);
        private:
            std::string name;
            Type* type;
        };
        std::string print_define() const;
        std::string print_declare() const;
    public:
        std::string print();
        /**
         * @return (i32 %a, ...) or ()
         */
        std::string arg_print_all() const;
        /**
         *
         * @return (i32, ...) or ()
         */
        std::string arg_print_type() const;
        static Function* Create(FunctionType* ty, const std::string& name, Module* module);

        [[nodiscard]] const std::string& get_name() const;

        [[nodiscard]] bool arg_empty() const;
        std::vector<std::unique_ptr<Arg> >::iterator arg_begin();
        std::vector<std::unique_ptr<Arg> >::iterator arg_end();
        /**
         * TODO: Remember to define these two static member
         */
    public:
        static Linkage ExternalLinkage;
        static Linkage InternalLinkage;
        void add_block(BasicBlock*);
    private:
        unsigned int v_reg_assigned;
        std::vector<std::unique_ptr<Arg> > arg_list;
        std::unordered_map<std::string, std::unique_ptr<BasicBlock> > b_list;

        // mem2reg prepared here :>
        std::unordered_map<std::string, std::unique_ptr<Alloca> > alloca_list;

        FunctionType* type;
        std::string name;

    };
    class GlobalVariable: public IRBase {
    public:
        Type* get_type();
        const std::string& get_name() const;
        bool isConstant() const;
    private:
        bool isConst;
    };

    /**
     *  For mem2reg preparation
     */
    class Alloca {
    public:
        static Alloca* Create(Type* ty, const std::string& name);
        static void Kill(Alloca* ptr);
        Type* get_type() const;
        const std::string& get_name() const;
    private:
        Alloca(Type* ty, const std::string& name);
        Type* type;
        std::string name;
        unsigned int v_reg;
        static std::list<std::unique_ptr<Alloca>> alloca_list;
    };


    /**
     *  Wait to be filled :)
     *  class Inst is the instruction unit, so each has a v_reg and evaluated flag
     *
     */

    class Inst {
    public:
        Inst();
        virtual std::string print(unsigned int &st) = 0;
        virtual Type* get_type() const = 0;
        bool isEvaluated() const;
        unsigned int getVReg() const;
        bool isConstant() const;
        Constant* get_con_ptr() const;
    protected:
        static std::vector<std::unique_ptr<Inst> > inst_list;
        unsigned int v_reg;
        bool evaluated;
        bool isConst;
        Constant* con_ptr;
    };


    class BranchInst: public Inst {
    public:
        std::string print(unsigned int &st) override;
        Type* get_type() const override;
        static Inst* Create(BasicBlock* Des);
        static Inst* Create(IRBase* con, BasicBlock* t_des, BasicBlock* f_des);
    private:
        explicit BranchInst(BasicBlock* Des);
        explicit BranchInst( IRBase* con, BasicBlock* t_des, BasicBlock* f_des);
        bool isConBr;
        BasicBlock* t_des;
        BasicBlock* f_des;
        Inst* con;
    };

    /**
     *  lhs and rhs remain IRBase*, because maybe the operands are constant.
     */
    class ArithInst: public Inst {
    public:
        std::string print(unsigned int &st) override;
        Type* get_type() const override;
        static Inst* Create(ARITH_TYPE, IRBase*, IRBase* );
    private:
        ArithInst(ARITH_TYPE, IRBase*, IRBase* );
        ARITH_TYPE op;
        IRBase* lhs;
        IRBase* rhs;
    };

    class AllocaInst: public Inst {
    public:
        std::string print(unsigned int &st) override;
        Type* get_type() const override;
        static Inst* Create(Type* ty, const std::string& name);
        static Inst* Store(IRBase* val, Alloca* ptr);
        static Inst* Load(Alloca* ptr, IRBase* val);
        Alloca* get_alloca();
    private:
        AllocaInst(ALLOCA_TYPE op, Alloca* ptr, IRBase* val);
        AllocaInst(ALLOCA_TYPE op, Alloca* ptr);
        Alloca* ptr;
        ALLOCA_TYPE op;
        IRBase* val;
    };

    class CmpInst: public Inst {
    public:
        std::string print(unsigned int &st) override;
        Type* get_type() const override;
        static Inst* Create(CMP_TYPE _op, IRBase* lhs, IRBase* rhs);
    private:
        CmpInst(CMP_TYPE _op, IRBase* LHS, IRBase* RHS);
        CMP_TYPE op;
        IRBase* lhs;
        IRBase* rhs;

    };

    class LogicInst: public Inst {
    public:
        std::string print(unsigned int &st) override;
        Type* get_type() const override;
        static Inst* Create(LG_TYPE _op, IRBase* lhs, IRBase* rhs);
    private:
        LogicInst(LG_TYPE _op, IRBase* lhs, IRBase* rhs);
        LG_TYPE op;
        IRBase* lhs;
        IRBase* rhs;

    };

    class RetInst: public Inst {
    public:
        std::string print(unsigned int &st) override;
        Type* get_type() const override;
        static Inst* Create(IRBase* val = nullptr);
    private:
        explicit RetInst(IRBase* val);
        IRBase* ret_val;
    };

};


#endif //SYSY_IRGEN_H
