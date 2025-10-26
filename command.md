# LAL manual

## 数学命令

### 矩阵创建与操作

- 关键词预览：
```
eye zeros ones rand randn diag linspace logspace meshgrid repmat 
reshape cat horzcat vertcat blkdiag tril triu flip rot90 compan 
```

```
1.eye ::varn {}              ==> eye ::A {3}         % 单位矩阵 **
2.zeros ::varn {}            ==> zeros ::A {3}       % 零矩阵 **
3.ones ::varn {}             ==> ones ::A {3}        % 全1矩阵 **
4.random ::varn {}           ==> random ::A {3}      % 均匀分布随机矩阵 *
5.randn ::varn {}            ==> randn ::A {3}       % 正态分布随机矩阵
6.diag -opt :varn ::varn     ==> diag -m :A ::B      % 对角矩阵或获取对角线元素 **
7.linspace   % 线性间隔向量 **
8.logspace   % 对数间隔向量
9.meshgrid   % 生成网格坐标
10.repmat :varn ::varn       ==> repmat :A ::B       % 复制和平铺矩阵 *
11.reshape   % 改变矩阵形状 *
12.cat       % 连接数组 *
13.horzcat   % 水平连接 *
14.vertcat   % 垂直连接 *
15.blkdiag   % 块对角矩阵
16.tril -opt :varn ::varn    ==> tril :A ::B         % 下三角部分 *
17.triu -opt :varn ::varn    ==> tilu :A ::B         % 上三角部分 *
18.flip -opr :varn ::varn    ==> flip -s :A ::B      % 翻转矩阵 *
19.rot90 -opt :varn ::varn   ==> rot90 -s :A         % 矩阵旋转90度 *
20.compan :varn ::varn       ==> compan :p ::A       % 伴随矩阵
```

### 矩阵基本信息

- 关键词预览：
```

```
size length ndims numel isscalar isvector ismatrix isempty isequal 
isnan isinf isfinite find nnz nonzeros issymmetric ishermitian isdefinite 
```
1.size :varn ::varn      ==> size :A ::s         % 矩阵维度 **
2.length :varn ::varn    ==> length :A ::len     % 最大维度长度 **
3.ndims :varn ::varn     ==> ndims :A ::num      % 维度数量
4.numel :varn ::varn     ==> numel :A ::num      % 元素个数 **
5.isscalar :varn         ==> isscalar :A         % 是否为标量 **
6.isvector :varn         ==> isvector :A         % 是否为向量 **
7.ismatrix :varn         ==> ismatrix :A         % 是否为矩阵 **
8.isempty :varn          ==> isempty :A          % 是否为空矩阵 **
9.isequal :varn :varn    ==> isequal :A :B       % 判断矩阵是否相等
10.isnan :varn           ==> isnan :A            % 检查NaN值
11.isinf :varn           ==> isinf :A            % 检查无穷大
12.isfinite  % 检查有限值
13.find      % 查找非零元素索引
14.nnz :varn ::varn      ==> nnz :A ::cnt        % 非零元素个数 **
15.nonzeros  % 非零元素值
16.issymmetric :varn     ==> issymmetric :A      % 是否为对称矩阵 *
18.ishermitian :varn     ==> ishermitian :A      % 是否为Hermitian矩阵 *
19.isdefinite :varn      ==> isdefinite :A       % 是否为正定矩阵
```

### 矩阵运算

- 关键词预览：
```
transpose ctranspose plus minus mtimes times mpower power mldivide 
mrdivide divide cross dot sum mean max min sort kron diff gradient 
```

```
1.transpose -opt :varn ::varn       ==> transpose -s :A       % 转置 **
2.ctranspose -opt :varn ::varn      ==> ctranspose -s :A      % 共轭转置 ('运算符) **
3.plus :varn :varn :varn ::varn     ==> plus :A :B :C ::D     % 加法 (+) **
4.minus -opt :varn :varn ::varn     ==> minus :A :B ::C       % 减法 (-) **
5.mtimes :varn :varn :varn ::varn   ==> mtines :A :B :C ::D   % 矩阵乘法 (*) **
6.times :varn :varn :varn ::varn    ==> times :A :B :C ::D    % 逐元素乘法 (.*) **
7.mpower :varn {}                   ==> mpower :A {3}         % 矩阵幂 (^) **
8.power :varn :varn ::varn          ==> power :A :B ::C       % 逐元素幂 (.^) **
9.mldivide   % 左除 (\) **
10.mrdivide  % 右除 (/) **
11.divide :varn :varn ::varn        ==> divide :A :B ::C      % 逐元素除 **
13.cross :varn :varn ::varn         ==> cross :A :B ::C       % 向量叉积 **
14.dot :varn :varn ::varn           ==> dot :A :B ::C         % 向量点积 **
15.sum -opt :varn ::varn            ==> sum -a :A ::total     % 元素求和 **
16.mean -opt :varn ::varn           ==> mean -a :A ::m        % 平均值 **
17.max -opt :varn ::varn            ==> max -a :A ::maxNum    % 最大值 **
18.min -opt :varn ::varn            ==> min -a :A ::minNum    % 最小值 **
19.sort -opt :varn ::varn           ==> sort -s :A ::B        % 排序
20.kron      % Kronecker 张量积
21.diff      % 数值差分运算
22.gradient  % 数值梯度计算
```

### 矩阵分解

- 关键词预览：
```
lu qr chol svd eig schur hess qz ldl 
```

```
1.lu :varn ::varn ::varn    ==> lu :A ::L ::U     % LU 分解 **
2.qr :varn ::varn ::varn    ==> qu :A ::Q ::R     % QR 分解 **
3.chol        % Cholesky 分解 **
4.svd         % 奇异值分解 **
5.eig         % 特征值分解 **
6.schur       % Schur 分解
7.hess        % Hessenberg 形式
8.qz          % QZ 分解（广义特征值）
9.ldl         % LDL分解
```

### 线性方程组求解

- 关键词预览：
```
linsolve inv pinv lscov rref null orth lsqnonneg lsqminnorm 
```

```
1.linsolve :varn :varn ::varn    ==> linsolve :A ::x :b  % 线性方程组求解 **
2.inv -opt :varn ::varn          ==> inv -s :A ::B       % 矩阵求逆 **
3.pinv       % 伪逆（Moore-Penrose） **
4.lscov      % 最小二乘解（已知协方差）
5.rref -opt :varn ::varn         ==> rref -s :A ::B      % 简化行阶梯形式 **
6.null       % 零空间基 *
7.orth       % 正交化 *
8.lsqnonneg   % 非负最小二乘 ** 
9.lsqminnorm % 最小范数最小二乘
```

### 特征值与奇异值

- 关键词预览：
```
poly condeig balance polyeig ordeig 
```

```
1.poly       % 特征多项式
2.condeig    % 特征值条件数
3.balance    % 矩阵平衡（改善特征值计算）
4.polyeig    % 多项式特征值问题
5.ordeig     %拟三角矩阵特征值
```

### 矩阵分析与条件数

- 关键词预览：
```
det rank trace norm cond condest rcond condeig balance 
```

```
1.det -opt :varn ::varn     ==> det -a :A ::d       % 行列式 **
2.rank -opt :varn ::varn    ==> rank -a :A ::r      % 矩阵的秩 **
3.trace -opt :varn ::varn   ==> trace -a :A ::t     % 矩阵的迹 **
4.norm       % 矩阵或向量范数 **
5.cond       % 矩阵条件数 **
6.condest    % 1-范数条件数估计
7.rcond      % 条件数倒数估计
8.condeig    % 特征值条件数
9.balance    % 矩阵平衡
```

### 矩阵函数

- 关键词预览：
```
expm logm sqrtm funm expm1 log1p 
```

```
1.expm       % 矩阵指数
2.logm       % 矩阵对数
3.sqrtm      % 矩阵平方根
4.funm       % 一般矩阵函数
5.expm1      % exp(A)-I的精确计算
6.log1p      %log(1+A)的精确计算
```

### 特殊矩阵

- 关键词预览：
```
gallery hadamard hankel hilb invhilb magic pascal rosser toeplitz vander wilkinson compan
```

```
1.gallery    % 测试矩阵集合
2.hadamard   % Hadamard 矩阵
3.hankel     % Hankel 矩阵
4.hilb       % Hilbert 矩阵
5.invhilb    % Hilbert 矩阵的逆
6.magic      % 魔方矩阵
7.pascal     % Pascal 矩阵
8.rosser     % Rosser 矩阵
9.toeplitz   % Toeplitz 矩阵
10.vander    % Vandermonde 矩阵
11.wilkinson % Wilkinson 特征值测试矩阵
12.compan    % 伴随矩阵
```

### 迭代法和预处理

- 关键词预览：
```
pcg bicg bicgstab cgs gmres lsqr minres qmr 
symmlq  ilu ichol gmres lsqr minres bicgstab 
```

```
1.pcg        % 预处理共轭梯度法
2.bicg       % 双共轭梯度法
3.bicgstab   % 双共轭梯度稳定法
4.cgs        % 共轭梯度平方法
5.gmres      % 广义最小残差法
6.lsqr       % LSQR 方法
7.minres     % 最小残差法
8.qmr        % 拟最小残差法
9.symmlq     % 对称LQ方法
10.ilu       % 不完全LU分解
11.ichol     % 不完全Cholesky分解
12.gmres     % 广义最小残差法
13.lsqr      % LSQR方法
14.minres    % 最小残差法
15.bicgstab  % 双共轭梯度稳定法
```


## 基本命令

- 关键词预览：
```
show 
```

```
1.show -opt   ==>   show -a
```


## 常量宏