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

## llvm

LLVM 工具链里有很多有意思的工具，下面我们会选择其中对实验较为重要的几个进行介绍。这一部分会涉及到后面的知识，看不懂其中的一些地方是正常的。当然如果你对 LLVM 工具链和 LLVM IR 比较熟悉，可以直接跳过此节。

在开始介绍前，让我们先编写一个最简单的 a + b 程序作为测试文件。
```cpp
//main.c
int main(){
    int a = 19260817;
    int b = 42;
    return a + b;
}
```
### Clang

Clang 作为 LLVM project 的一个子项目，是 LLVM 项目中 c/c++/obj-c 语言的前端，其用法与 GCC 基本相同。

你可以输入 `clang -help` 查看支持的所有选项。

预计将在实验中用到的指令有以下几个：

```bash
# 生成可执行文件
$ clang main.c -o main
# 查看编译的过程
$ clang -ccc-print-phases main.c

# 生成 tokens
$ clang -E -Xclang -dump-tokens main.c
# 生成语法树
$ clang -fsyntax-only -Xclang -ast-dump main.c
# 生成 llvm ir（不开优化）
$ clang -S -emit-llvm main.c -o main.ll -O0

# 生成汇编（在本实验中用处不大）
$ clang -S main.c -o main.s
# 生成目标文件（在本实验中用处不大）
$ clang -c main.c -o main.o
```

请尝试在命令行中输入这些命令，并看看都输出了什么。我们会在后面对其进行详细介绍。

### lli

`lli` 会解释或以 JIT 的形式运行 `.bc` 和 `.ll` 程序。其中，`.bc` 是 LLVM IR 的二进制格式（方便存储），`.ll` 是 LLVM IR 的可读代码格式（给人看的）。

在实验中，我们只需要用最简单的形式直接使用指令即可。以 `main.c` 为例：

```bash
# 1. 生成 main.c 对应的 .ll 格式的文件
$ clang -S  -emit-llvm main.c -o main.ll -O0

# 2. 用 lli 解释执行生成的 .ll 文件
$ lli main.ll
```

如果一切正常，查看上一条指令的返回值，你会看到：

```bash
$ echo $?
187  # 即 (19260817 + 42) % 256
```

### llvm-link
lli 仅能运行单个 `.ll` 或 `.bc` 格式的文件，当我们想要使用别的库的时候，就需要用到 llvm-link。

在本实验中，我们引入了 `libsysy` 库（在这里可以看到）为我们的程序提供 IO 方面的操作。

例如，我们想要输出一个 int 的值，则需要用到 `putint()` 这个函数。我们可以直接在 main.c 中调用它：

```cpp
int main() {
    int a = 19260817;
    int b = 42;

    putint(a);

    return a + b;
}
```

如果我们按照上面的方法直接用 lli 解释由这个 main.c 导出得到的 main.ll，会发现程序产生了错误：

```bash
$ lli main.ll
PLEASE submit a bug report to https://bugs.llvm.org/ and include the crash backtrace.
Stack dump:
1.      Program arguments: lli main.ll
zsh: segmentation fault (core dumped)  lli main.ll
```
这是因为 lli 只解释了 main.ll 这一个文件，但是找不到库函数 `putint` 在哪。此时就需要用 llvm-link 来进行链接。

`llvm-link` 能够将多个 `.ll` 或 `.bc` 格式的文件链接为一个文件，例如：

```bash
# 1. 分别导出 libsysy 和 main.c 对应的的 .ll 文件
$ clang -emit-llvm -S libsysy.c -o lib.ll
$ clang -emit-llvm -S main.c -o main.ll

# 2. 使用 llvm-link 将两个文件链接，生成新的 IR 文件
$ llvm-link main.ll lib.ll -S -o out.ll

# 3. 用 lli 解释运行
$ lli out.ll
19260817
```

### 其他有用的工具
- `llc`：可以将 `.ll` 形式的 `IR` 编译成指定体系结构的汇编

- `opt`：LLVM 模块化的优化器和分析器。它以 LLVM 源文件为输入，对其运行指定的优化或分析，然后生成优化文件或输出分析结果。这个工具将会在挑战任务中介绍，在此不再展开。

New Bing YYDS|:)