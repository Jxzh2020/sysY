## Platform

This is a cmake project, which I believe it's working on Windows.
To build on Windows, `LLVM` , `clang` and `cmake` is required.
Once all the dependencies is ready, Visual Studio or Code is fine to edit the project.

## git

首先，配置本地git设置，使其能通过ssh连接github远程仓库。
`git clone` 项目, 获取一份项目副本

`git checkout -b [branch name]` 新建分支，同时切换至该分支

进行一些修改，现在想要同步到远程仓库

`git add *` 将所有文件保存到暂存区
`git status` 查看当前的git文件状态
`git commit -m "some info"` 将暂存区的代码提交到本地仓库

`git push -u origin branch_name`
将branch_name分支推送到origin远程仓库，
新分支首次需要-u，之后不需要），`branch_name` 可以不写，默认就是当前分支。
比如： `git push origin`

> Tip：
> `git checkout branch_name`可以切换分支
> 为了同步远程仓库，时不时使用 `git fetch` 命令
> 各自在各自分支写作，时不时通过`git merge`合并到主分支后再同步到各自分支，或者直接各自分支相互merge，确定没问题后再merge主分支


## Project

```bash
# Ast related classes and IR Genreation details
./include/Ast
./src/Ast
# A Replacement of LLVM API Implementation
./include/IRGen
./src/IRGen

```

A glimpse of the IR structure is defined in `BaseAST::astJson()` function, where custom ir is printed.
To generate a `llvm ir` in=memory form, use `BaseAST::codegen()`.

Current Supported Grammar in ENBF:

```bash
    CompUnit    ::= [CompUnit] (Decl | FuncDef);
    FuncDef     ::= PrimitiveType IDENT "(" [FuncFParams] ")" Block;
    PrimitiveType    ::= "void" | "int";

    UnaryExp    ::= PrimaryExp | UnaryOp UnaryExp | IDENT "(" [FuncRParams] ")";

    Number      ::= INT_CONST;
    UnaryOp     ::= "+" | "-" | "!";
    MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
    AddExp      ::= MulExp | AddExp ("+" | "-") MulExp;

    Exp         ::= LOrExp;
    RelExp      ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
    EqExp       ::= RelExp | EqExp ("==" | "!=") RelExp;
    LAndExp     ::= EqExp | LAndExp "&&" EqExp;
    LOrExp      ::= LAndExp | LOrExp "||" LAndExp;

 Trying to eliminate some AST derived class:
 Logic Expression: LgExpAST containing LOrExp, LAndExp, EqExp
 Logic Operation:  OR ||, AND &&,

 Comparison Expression: CompExpAst containing RelExp
 Comparison Operation: LT <, GT >, LE <=, GE >=, EQ ==, NEQ !=

    ConstDef      ::= IDENT "=" ConstInitVal;
    ConstInitVal  ::= ConstExp;
    ConstExp      ::= Exp;
    LVal          ::= IDENT;
    BlockItem     ::= Decl | Stmt;
    PrimaryExp    ::= "(" Exp ")" | LVal | Number;


    ConstDecl     ::= "const" PrimitiveType ConstDef ConstDefs ";";
    ConstDefs     ::= ConstDefs "," ConstDef | ;
    Block         ::= "{" BlockItems "}";
    BlockItems    ::= BlockItems BlockItem | ;


    Decl          ::= ConstDecl | VarDecl;
    VarDef        ::= IDENT | IDENT "=" InitVal;
    VarDecl       ::= PrimitiveType VarDef {"," VarDef} ";";
    InitVal       ::= Exp;

    Stmt          ::= Matched 
    
                      
    Matched       ::= LVal "=" Exp ";"
                    | [Exp] ";"
                    | Block
                    | "return" [Exp] ";"
                    | IF '(' Exp ')' Matched Else Matched
                    | "while" "(" Exp ")" Stmt
                    | "break" ";"
                    | "continue" ";" ;
    
    Unmatched     ::= IF '(' Exp ')' Stmt 
                    | IF '(' Exp ')' Matched ELSE Unmatched
                    

    FuncDefs      ::= FuncDefs FuncDef | ;
    FuncFParam    ::= PrimitiveType IDENT;
    FuncFParams   ::= FuncFParam | DFuncFParams ',' FuncFParam | ;
    DFuncFParams  ::= DFuncFParams ',' FuncFParam | FuncFParam ;
    FuncRParams   ::= Exp {"," Exp};
```

### Ast

The base class is `BaseAST`, namely all the nodes being derived class of it.