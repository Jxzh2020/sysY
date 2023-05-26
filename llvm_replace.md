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

Virtual register is assigned to Inst in constructor. Constant value Inst node is not assigned. Use isConst to verify.

Now here's the thing: v_reg is set at the beginning of an Inst object( or not if Inst is ConstExp)
Among all the Inst objects, some is inserted into basicblocks, while others don't.
But at Builder level, all the Inst objects are inserted into the Inst vector.
This means, **Inst::print() itself should be able to decide**, if it's going to return some string
to the file.

Of course, if we don't consider any optimization, all the inst are output to IR file, it's ok.

The syncing of v_reg and value, in me opinion, is unnecessary. Inst first check if it's constant exp
then return std::string according to v_reg. Inst::value is dumped.


Now,
1. Adding v_reg support on Create and constructors, modifying Inst::print() --done
2. there maybe type cast? So this part should move to constructor -- leave unsolved
3. remove Inst::val --done
4. modifying Inst::get_value() --done

Alloca::Load and Builder::CreateLoad is useless.

## Array support

Now, this project supports sysY basic features except array type. And Function Argument is solved in a sloppy way.

Inorder to support array, not only a new kind of Inst class is needed, the whole arg system needs refactor, too.

```c
void h(int input[]){
    int *start = input;
    start[0] = 1;
}
```

It seems that `store ptr %0, ptr %2` simply assign `%2` with `%0`, which makes current arg system possible.
```llvm
define void @h(ptr %0) {
  %2 = alloca ptr
  %3 = alloca ptr
  store ptr %0, ptr %2
  %4 = load ptr, ptr %2
  store ptr %4, ptr %3
  %5 = load ptr, ptr %3,
  %6 = getelementptr inbounds i32, ptr %5, i64 0
  store i32 1, ptr %6
  ret void
}
```

**Maybe** the arg system need no modification. Just add support a new type `ptr` and a New Inst `getelementptr`