# Schedule

branch *dev* is meant to implement all the LLVM reference in original project *sysY*

- Class `IRGen` or Namespace `IRGen`? 

    Maybe namespace first.

## structure
reviewing arc/Ast
now at StmtAST-codegen
- how is global decl of const implemented

```text
IRGen:
    Module
        getGlobalVariable(string name)
    Builder
        CreateBr( BasicBlock* )
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
        CreateICmpSGT
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
    BasicBlock <- IRBase
        get_type : void
        get_name : block name
        Create( string name , Function* )
    Function <- IRBase
        get_type : return type
        get_name : function name
        Create( FunctionType* , Linkage , string name, Module)
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
    
llvm::APInt  ? // what this is ?
```