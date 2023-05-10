### how is the basic block in llvm ir created?

According to the web search results, a basic block in LLVM IR is a container of instructions that execute sequentially.
To create a basic block, you can use the `BasicBlock::Create` method and pass the context, the name of the block and the
function containing the block. You can also use an IRBuilder object to create and append instructions to the end of a
block. Here is a code example of creating a basic block:

```cpp
// Create a function
FunctionType *FT = FunctionType::get(Type::getVoidTy(context), false);
Function *F = Function::Create(FT, Function::ExternalLinkage, "foo", module);

// Create a basic block
BasicBlock *BB = BasicBlock::Create(context, "entry", F);

// Create an IR builder
IRBuilder<> builder(BB);

// Create some instructions
Value *X = builder.CreateAlloca(Type::getInt32Ty(context), nullptr, "x");
Value *Y = builder.CreateAlloca(Type::getInt32Ty(context), nullptr, "y");
Value *Z = builder.CreateAdd(X, Y, "z");
builder.CreateRet(Z);
```

### how is a `return` statement in llvm ir?

To create a return statement using the LLVM C++ API, you can use the `ReturnInst::Create` method and pass the context,
the value to return (or nullptr for void) and the basic block to insert at. For example:

```cpp
// Create a context
LLVMContext context;

// Create a basic block
BasicBlock *BB = ...;

// Create a return statement
ReturnInst::Create(context, nullptr, BB); // ret void
```

To return 0, you can,

```cpp
// Create a context
LLVMContext context;

// Create a basic block
BasicBlock *BB = ...;

// Create a return 0 statement
ReturnInst::Create(context, ConstantInt::get(Type::getInt32Ty(context), 0), BB); // ret i32 0
```

### how to print the llvm ir in-memory form?

According to the web search results, there are different ways to print the LLVM IR in-memory form depending on your
needs. One way is to use the `Module::print` method and pass an output stream and an optional assembly annotation
writer1. For example:

```cpp
// Create a module
Module *M = ...;

// Create an output stream
raw_ostream &OS = ...;

// Print the module
M->print(OS, nullptr); // print LLVM IR to output stream
```

### How can I create a `raw_ostream` object?

- To create a `raw_ostream` object that outputs to the standard output stream, you can use the `llvm::outs` function,
  which returns a reference to a `raw_ostream` object. For example:

    ```cpp
    // Create a raw_ostream object that outputs to stdout
    raw_ostream &OS = llvm::outs();
    ```
- To create a raw_ostream object that outputs to a file, you can use the `llvm::raw_fd_ostream` class, which is a
  subclass of raw_ostream that takes a file descriptor as a parameter. For example:

    ```cpp
    // Create a raw_ostream object that outputs to a file
    std::error_code EC;
    raw_fd_ostream OS("output.txt", EC); // open file "output.txt" for writing
    ```

- To create a `raw_ostream` object that outputs to a string, you can use the `llvm::raw_string_ostream` class, which is
  a subclass of raw_ostream that takes a reference to a string as a parameter. For example:

    ```cpp
    // Create a raw_ostream object that outputs to a string
    std::string Str;
    raw_string_ostream OS(Str); // write to string Str
    ```

### Why there's only Neg unary operation for floating-point number?

//

### how to do constant expression in llvm ir?

根据搜索结果，你可以使用`ConstantExpr`类来创建一个常量表达式，它是一个使用其他常量值的表达式来初始化的常量值。常量表达式使用标准的指令操作码来定义各种常量表达式。例如：

%const = add i32 4, 2 %expr = getelementptr i32, i32* %const, i32 1

常量表达式可以用于初始化全局变量，或者作为其他常量表达式的操作数。常量表达式也可以用于某些指令的操作数，例如switch和ret。

### how to lookup an alloca value safely in llvm ir using c++ api?

To lookup an alloca value safely in llvm ir using c++ api, you need to cast the llvm::Value* to llvm::AllocaInst* using
the LLVM RTTI system1. For example:

```cpp
llvm::Value* val = someFunction(...); // val is an alloca value
    if (llvm::AllocaInst* I = llvm::dyn_cast<llvm::AllocaInst>(val)) {
    // do something with the alloca instruction I
    }
    else {
    // not an alloca instruction
    }
```
