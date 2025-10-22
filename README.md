# LinearAlgebraLab(LAL)

此项目是由纯 C++ 实现，并使用 `Laxb` 语言(类Linux shell)，运行在 CLI 窗口，进行 `线性代数` 领域的各种 `数值` 与 ~~符号~~ 计算的 `线性代数实验室` (LAL)

```cpp
项目启动时间：20250909
预期结束时间：20260104
项目结束时间：2026
```


## 功能特性

- 支持矩阵基本代数运算（加法、乘法、转置等）
- 矩阵分解（LU、QR、Cholesky、SVD等）
- 线性方程组求解（A**x** = **b**）
- 行列式、秩、逆矩阵等计算
- 工作空间持久化（保存和加载会话）
- 交互式命令行界面（于 Windows 的 cmd/PowerShell 窗口的 Laxb 语言）


## 项目结构

### 运行流程

```
用户输入 "matrix M 3 4;"
    ↓
LaxbIO 解析这个命令
    ↓  
LaxbIO 调用 LALCore 创建3行4列矩阵
    ↓
LaxbIO 调用 Workspace 保存矩阵A至.lal文件/内存
    ↓
Workspace 把矩阵A存到内存，需要时保存到文件
```

### 文件结构

```
.\LALCore\
├─include
│  ├─core
│  │      matrix.hpp
│  │      vector.hpp
│  │      
│  ├─decompositions
│  │      advanced_decomp.hpp
│  │      basic_decomp.hpp
│  │      
│  ├─solvers
│  │      eigen_solvers.hpp
│  │      least_squares_solver.hpp
│  │      linear_solvers.hpp
│  │
│  ├─spaces
│  │      projections.hpp
│  │      subspaces.hpp
│  │
│  └─utils
│          factory.hpp
│          functions.hpp
│          numerical.hpp
│          output.hpp
│
└─src
    │  main.cpp
    │
    ├─core
    │      matrix.cpp
    │      vector.cpp
    │
    ├─decompositions
    │      advanced_decomp.cpp
    │      basic_decomp.cpp
    │
    ├─solvers
    │      eigen_solvers.cpp
    │      least_squares_solver.cpp
    │      linear_solvers.cpp
    │
    ├─spaces
    │      projections.cpp
    │      subspaces.cpp
    │
    └─utils
            factory.cpp
            functions.cpp
            numerical.cpp
            output.cpp
```


## 项目风格

### 注释风格

- 文件头

```cpp
// ============
// # context
// - 
// - 
// ============
```

- 函数、类头1

```cpp
/* ======== context ======== */
```

- 函数、类头2

```cpp
/* context
 * - context */
```

- 普通小段注释1

```cpp
// context
```

- 普通小段注释2

```cpp
// context
// context
```

### 代码风格

- 普通变量、对象、函数首字母小写驼峰命名：getCol();
- 类、枚举等首字母大写驼峰命名：class Matrix {...};
- const、宏全大写蛇形命名：MATH_PI
- 命名多为单词全拼，部分经典词汇用缩写，这导致命名清晰却很长


## 使用指南

### 启动程序

- 未设置环境变量时
```bash
.\LAL arg_v ...
```

- 添加 `.\LAL` 为环境变量时
```bash
LAL arg_v ...
```

### 基本语法

语法与 Linux 的 `bash` 操作相似

```
command -opt :varn ::varn {a b c} [a b c; d e f];
```

- 命令后三种参数的 `位置、数量没有要求`
- 输入每一句命令后按下 `Enter` 默认立即输出
- 语句末尾加 `;` 时抑制输出

|-opt|::varn|:varn|[list]|{list}|
|---|---|---|---|---|
|接 `opt` 选项|将结果 `重定向输出` 至此变量|被操作变量|`矩阵元素` 列表|`常数参数` 列表|

`opt` 有以下几种类型（不包括特殊类型，如 -m 为主对角线）：

|-s|||||
|---|---|---|---|---|
|操作将改变自身（默认所有操作不改变自身）|||||


### 常用命令与宏

#### 命令(`差色词`表示可选)

基本命令

|命令|简单说明|示例|
|:-------------------------------------|:-----------------------------------------------|:-----------|
|manual `[obj]`|命令手册，无参数时简洁输出手册所有内容，有参数时输出对应详细条目|manual det|
|clear `[obj]`| 存在 [obj] 时：清除 [obj] 的变量对象内容|clear ALL_VAR|
|show `[-opt]` `[obj1]` `...` `[objn]` |有可选参数时：根据 [-opt] 展示对应对象|show -f M b|
|save|主动保存此次工作空间至 `workspace`|save|
||||

- 声明变量命令(默认初始化为0)

|命令|简单说明|示例|
|:----------------------------------------|:---------------------------------------|:---------------------------|
|num [name] `[init_value]`|创建数字变量，可以存储整型或浮点型|num var 8|
|matrix [name] [row] [col] `[init_value]`|创建矩阵变量，存储 m*n 矩阵，初始化内容需方括号括起来，每行结尾为 ';'。单行/列矩阵会退化为vector型|matrix A 2 3 [1 2 3; 4 5 6]|
|vector [-opt] [name] [length] `[init_value]`|创建向量变量，存储单行/列向量，初始化内容需要方括号括起来，[-opt]有两个选项，行向量-r，列向量-c|vector -c x 4 [1 3 -25 10.3]|

- 数学命令

|命令|简单说明|示例|
|:---|:----|:---|
||||

#### 宏

|宏|简单说明|示例|
|:---|:----------------------|:--------|
|TMP[]|变量，可以转化为任意类型，仅用于存储临时数据。方括号为下标，使用方式与CPP内置数组一致|show TMP[]|
|ALL_VAR|表示所有变量|show ALL_VAR|
||||

- 更多命令、宏及其详细介绍请使用`manual`命令查看

## 文件格式

LAL 使用 `JSON` 格式保存工作空间文件(.lal 扩展名)，示例内容如下：

```json

```


## 致谢

- 感谢 Gilbert Strang 教授的线性代数课程启发

---