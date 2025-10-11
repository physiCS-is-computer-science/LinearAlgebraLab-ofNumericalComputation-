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
lal/LALCore/
│  command.md
│  README.md
│
├─LALCore
│  ├─include/
│  │  ├─core/ # 基础数据结构层
│  │  │      matrix.hpp # 矩阵 类
│  │  │      vector.hpp # 向量 类
│  │  │
│  │  ├─decompositions/ # 矩阵分解模块
│  │  │      cholesky_decomposition.hpp # Cholesky分解 类
│  │  │      eigen_decomposition.hpp # 特征值分解 类
│  │  │      LU_decomposition.hpp # LU分解 类
│  │  │      QR_decomposition.hpp # QR分解 类
│  │  │      schur_decomposition.hpp # Schur分解 类
│  │  │      SVD.hpp # 奇异值分解 类
│  │  │
│  │  ├─eigen/ # 特征系统模块
│  │  │      eigen_solver.hpp # 特征求解器 类
│  │  │      generalized_eigen.hpp # 广义特征值
│  │  │      power_method.hpp # 幂方法
│  │  │      QR_algorithm.hpp # QR算法
│  │  │
│  │  ├─factory/ # 矩阵创建与工厂模块
│  │  │      matrix_builder.hpp # 矩阵构建工具
│  │  │      matrix_factory.hpp # 特殊矩阵生成器
│  │  │      random_generator.hpp # 随机矩阵生成器
│  │  │
│  │  ├─functions/ # 矩阵函数模块
│  │  │      matrix_exponential.hpp # 矩阵指数
│  │  │      matrix_logarithm.hpp # 矩阵对数
│  │  │      matrix_power.hpp # 矩阵幂
│  │  │      matrix_square_root.hpp # 矩阵平方根
│  │  │      matrix_trigonometric.hpp # 矩阵三角函数
│  │  │
│  │  ├─least_squares/ # 最小二乘与优化模块
│  │  │      least_squares_solver.hpp # 最小二乘求解器 类
│  │  │      regularized_least_squares.hpp # 正则化最小二乘
│  │  │      weighted_leadt_squares.hpp # 加权最小二乘
│  │  │
│  │  ├─numerical/ # 数值稳定性模块
│  │  │      condition_estimator.hpp # 条件数估计
│  │  │      error_analysis.hpp # 数值误差分析
│  │  │      numerical_rank.hpp # 数值秩判断
│  │  │      preconditioner.hpp # 预处理器
│  │  │
│  │  ├─solvers/ # 线性系统求解模块
│  │  │  │  linear_solver.hpp # 线性方程组求解器 基类
│  │  │  │
│  │  │  ├─direct/
│  │  │  │      cholesky_solver.hpp # Cholesky求解器 子类
│  │  │  │      gaussian_sollver.hpp # 高斯消元求解器 子类
│  │  │  │      LU_solver.hpp # LU求解器 子类
│  │  │  │      QR_solver.hpp # QR求解器 子类
│  │  │  │
│  │  │  └─iterative/
│  │  │          conjugate_gradient_solver.hpp # 共轭梯度求解器 子类
│  │  │          gauss_seidel_solver.hpp # Gauss-Seidel求解器 子类
│  │  │          GMRES_solver.hpp # GMRES求解器 子类
│  │  │          jacobi_solver.hpp # 雅可比求解器 子类
│  │  │
│  │  ├─subspaces/ # 子空间与投影模块
│  │  │      fundamental_subspaces.hpp # 四个基本子空间
│  │  │      orthogonal_projection.hpp # 正交投影
│  │  │      projector.hpp # 投影矩阵生成
│  │  │      subspace_angles.hpp # 子空间夹角
│  │  │
│  │  └─utilities/
│  │          constants.hpp # 数学常量
│  │          utilities.hpp # 公用工具
│  │
│  └─src/
│      │  main.cpp # 程序主循环
│      │
│      ├─core/
│      │      matrix.cpp
│      │      vector.cpp
│      │
│      ├─decompositions/
│      │      cholesky_decomposition.cpp
│      │      eigen_decomposition.cpp
│      │      LU_decomposition.cpp
│      │      QR_decomposition.cpp
│      │      schur_decomposition.cpp
│      │      SVD.cpp
│      │
│      ├─eigen/
│      │      eigen_solver.cpp
│      │      generalized_eigen.cpp
│      │      power_method.cpp
│      │      QR_algorithm.cpp
│      │
│      ├─factory/
│      │      matrix_builder.cpp
│      │      matrix_factory.cpp
│      │      random_generator.cpp
│      │
│      ├─functions/
│      │      matrix_exponential.cpp
│      │      matrix_logarithm.cpp
│      │      matrix_power.cpp
│      │      matrix_square_root.cpp
│      │      matrix_trigonometric.cpp
│      │
│      ├─least_squares/
│      │      least_squares_solver.cpp
│      │      regularized_least_squares.cpp
│      │      weighted_leadt_squares.cpp
│      │
│      ├─numerical/
│      │      condition_estimator.cpp
│      │      error_analysis.cpp
│      │      numerical_rank.cpp
│      │      preconditioner.cpp
│      │
│      ├─solvers/
│      │  ├─direct/
│      │  │      cholesky_solver.cpp
│      │  │      gaussian_sollver.cpp
│      │  │      LU_solver.cpp
│      │  │      QR_solver.cpp
│      │  │
│      │  └─iterative/
│      │          conjugate_gradient_solver.cpp
│      │          gauss_seidel_solver.cpp
│      │          GMRES_solver.cpp
│      │          jacobi_solver.cpp
│      │
│      ├─subspaces/
│      │      fundamental_subspaces.cpp
│      │      orthogonal_projection.cpp
│      │      projector.cpp
│      │      subspace_angles.cpp
│      │
│      └─utilities/
│              utilities.cpp
│
├─LaxbIO/ # Laxb的IO模块
│  ├─include/
│  │      command_manual_lib.hpp # 命令手册库
│  │      command_parser.hpp # 命令解析器 类
│  │
│  └─src/
│          command_parser.cpp
│
└─Workspace/ # 工作空间模块
    ├─include/
    │      workspace.hpp # 工作空间 类
    │
    ├─src/
    │      workspace.cpp
    │
    └─user_files/ # 被保存的.lal文件
            test.lal
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
./LAL arg_v ...
```

- 添加 `.\LinearAlgebraLab\x64\release` 为环境变量时
```bash
LAL arg_v ...
```

### 基本语法

语法与 Linux 的 `bash` 操作类似

```
command [-opt] [obj] ...
```

- 输入每一句命令后按下 `Enter` 默认立即输出
- 语句末尾加 `;` 时抑制输出

### 常用命令与宏

#### 命令(`差色词`表示可选)

- 基本命令

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