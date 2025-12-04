# LinearAlgebraLab(LAL)

此项目是由纯 C++ 标准库实现，并使用 `Laxb` 语言(类Linux shell)，运行在 CLI 窗口，进行`线性代数`领域中各种`数值`计算的`线性代数实验室`（LAL）

- 需要 Windows 环境
- 源码上传于 GitHub，点击[ [₍˄·͈༝·͈˄*₎◞*] ](https://github.com/physiCS-is-computer-science/LinearAlgebraLab-ofNumericalComputation-)浏览
- 预编译版本于`release`文件夹，双击`LAL.exe`即可运行（同目录下必须有`user_session_files`文件夹，否则会话保存功能失效）。使用`Cmake`可以手动编译
- 这是试验性项目，加之个人水平较差火，因而项目简陋`(T～T)`
- 如能给出批评建议，或发现 bug，不胜感激！`(*￣3￣)╭♡`

```cpp
项目启动时间：20250909
预期结束时间：20260104
项目结束时间：2026
```


## 功能特性

#### 目前支持的功能（数学层面）∑(❍ฺд❍ฺlll)：

1. 矩阵基本运算、操作、性质（如矩阵拼接分割、求逆等）
2. 两个子空间（转置得到另外两个）、投影 P = A(AᵀA)⁻¹Aᵀ（QR分解取Q构造P）
3. LU分解（高斯消元）、QR分解（豪斯霍尔德反射矩阵）
4. 线性方程组 Ax = b 求解、最小二乘求解 AᵀAx = Aᵀb（正规方程）。二者都输出通解 xp + xn，暂不支持最小范数解`(=v=)`

#### 目前支持的功能（非数学层面）：

1. 交互式命令行界面（于 Windows 中 cmd/PowerShell 窗口的 Laxb 语言）
2. 工作空间持久化（保存和加载会话）

#### 未来可能加入的（取决于个人时间）(╥╯﹏╰╥)ง：

1. SVD分解（奇异值）、特征值分解`＼(￣︶￣)／`
2. 基于最小二乘对散点图拟合函数图像（要求函数系数为线性或可以构造为线性，如 f(x) = e^(kx +b) 取对数可以得到线性系数）
3. 广义逆、二次型`:(`


## 项目结构

#### 运行流程

```
用户输入 "var ::A [1 2 3; 4 5 6];"
    ↓
LaxbIO 解析这个命令
    ↓  
LaxbIO 调用 LALCore 创建2行3列矩阵
    ↓
LALCore 调用 SessonManager 保存矩阵A至 .lal文件/内存，（如果计算失败则写入错误信息至 SessonManager）
    ↓
LALCore 根据 SessonManager 状态判断输出内容
```

#### 文件结构

```
./
├─LALCore
│  ├─include
│  │  ├─core
│  │  ├─decompositions
│  │  ├─solvers
│  │  ├─spaces
│  │  └─utils
│  └─src
│      ├─decompositions
│      ├─solvers
│      ├─spaces
│      └─utils
├─LaxbIO
│  ├─include
│  └─src
├─release
│  └─user_session_files
└─SessionManager
    ├─include
    └─src
```


## 使用指南

#### 启动程序

- 双击 LAL.exe
- 或添加路径 `.\LAL` 为环境变量（目前暂不支持传参给主函数），于命令行键入：
```bash
LAL arg_v ...
```

#### 基本语法

语法与 Linux 的 `shell` 操作相似：

```
command -opt :varn ::varn {a b c} [a b c; d e f];
```

- 命令名后的参数在**功能**上**对位置没有要求**，在**逻辑**上**部分有要求**（如减法左右一定是`有序的`）
- 输入每一句命令后按下 `Enter` 默认立即输出
- 语句末尾加 `;` 时抑制输出

|-opt|::varn|:varn|[list]|{list}|
|---|---|---|---|---|
|选项|将结果 `输出` 至此变量|`输入` 变量|`矩阵` 列表|`常数参数` 列表|

#### 示例

查看`linsolve`命令使用方式
```
[LAL #]<< help :linsolve
[LAL #]>:

linsolve        -opt :varn ::varn
---
# Solving linear equations Ax = b
-a:       The total solution of the output [xp xn] form of the matrix
:varn:    Accept a variable
          :A :b
::varn:   Output to this variable
          -a ::X
          ::x
```

保存当前会话空间
```
[LAL #]<< save ::test
[LAL #]>: "./user_session_files/test.lal" saved
```

qr分解
```
[LAL #]<< var ::A [1 2; 2 3; 3 4];
[LAL #]<< qr :A
[LAL #]>:

     -0.2673      0.8729      0.4082
     -0.5345      0.2182     -0.8165
     -0.8018     -0.4364      0.4082

     -3.7417     -5.3452
      0.0000      0.6547
      0.0000      0.0000
```

退出当前绑定的文件
```
[LAL test]<< quit -f
[LAL #]<<
```

- 更多命令及其详细介绍请使用`help`命令查看

#### 注意事项

`[LAL #]`后跟随标识符：
1. `<<`等待输入
2. `>:`正常输出
3. `>!`错误输出

`[LAL #]`中`#`表示当前为临时会话空间，当绑定加载文件（load -c）时会变为`[LAL file]`，此时的更改会同步作用于此文件

## 文件格式

LAL 使用特定文本格式的`.lal`扩展名保存工作空间文件，示例内容如下：

```
"A":[
           1           2           3;
           4           5           6;
],
"a":3.14159,
```


## 致谢

- 感谢 Gilbert Strang 教授的线性代数课程启发，这使我受益终生

---