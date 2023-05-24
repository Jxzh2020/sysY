# Schedule

branch *dev* is meant to implement all the LLVM reference in original project *sysY*

- Class `IRGen` or Namespace `IRGen`? 

    Maybe namespace first.

## structure
reviewing arc/Ast

- how is global decl of const implemented
- Module and Builder are manually killed
- some function does not influence the IR, they only read data
- For symbol table, it has to provide a unique name for user.
- It seems that the order of instruction is the same as the create-functions been called

Module maintains most of the context information, including global variables, functions, declarations and so on.
BasicBlock better be maintained in Function, but should provide interface for Builder to access

According to real usage, IRBase does not have to be the base class of all the class, most of the case.
Inst creation function and value function should return the IRBase*.

```text
IRGen:
    Module
        getGlobalVariable(string name)
        
    Builder
        CreateBr( BasicBlock* )
        CreateConBr( condition: IRBase* , true block: BasicBlock* , false block: BasicBlock* )
        
        CreateAdd
        CreateSub
        CreateNeg
        
        CreateNul( LHS: IRBase* , RHS: IRBase* )
        CreateSDiv
        CreateSRem
        
        CreateAlloca( Type*, string name)
        CreateStore( src: IRBase* , des: Alloca* )
        CreateLoad( src: Alloca* , des: IRBase* )
        
        CreateGlobalVariable(Module* , Type* , isConstant , Linkage , Initializer , string name)
        
        CreateICmpSLT( LHS: IRBase*, RHS: IRBase* )
        CreateICmpSGT
        CreateICmpSLE
        CreateICmpSGE
        CreateICmpEQ
        CreateICmpNE
        
        CreateLogicalOr( LHS , RHS )
        CreateLogicalAnd
        
        CreateRetVoid
        CreateRet( IRBase* )
        
        CreateCall( Function* ) for void parameter
        CreateCall( Function* , params: IRBase* ) for non-void parameter
        SetInsertPoint( BasicBlock* )
        
    Type
        getInt32
        getVoid
        getInt1 ? see how it works in LLVM IR // in StmtAST::bool_convert
        ...
        
    Constant
        get(Type* , value)
        
    FunctionType
        get( return_type: Type* , Parameters: vector<Type*> , isVarArg)
        
    IRBase
    
    BasicBlock
        get_name : block name
        set_name()
        Create( string name , Function* )
        
    Function <- IRBase
        Create( FunctionType* , Linkage , string name, Module)
        get_type : return type
        get_name : function name
        bool arg_empty()
        arg_begin()
        arg_end()
        Arg
            SetName(string name)  ? // not sure the necessity? may be when using them ?
            getName()
        ArgIter
        
    AllocaInst <- IRBase
        get_type : return alloca type
        get_name : return variable name
        get_AllocatedType()
    
IRGen::APInt  ? // what this is ?
```


***NOTE: there may be some error in AllocaInst, which should all return Alloca->IRBase***


**Funny thing!**
```cpp
class Inst {
    public:
        Inst();
        /**
         *  Really funny thing!
         */
        virtual ~Inst() {}
        virtual std::string print(unsigned int &st) = 0;
        virtual Type* get_type() const = 0;
        bool isEvaluated() const;
        unsigned int getVReg() const;
        bool isConstant() const;
        Constant* get_con_ptr() const;
```

## Refactor

1. Virtual register allocation now seems conflict with the building phase of IR.
Now planning to refactor Builder::CreateXXXX to let virtual register distributed at building IR phase.

2. Separating static Node and dynamic Node clearly.
There are some overlaps between AllocaInst and Alloca, since building tree phase need only the snapshot 
of Alloca, which is provided by both AllocaInst and Alloca itself.**So now planing to remove Alloca class** :)