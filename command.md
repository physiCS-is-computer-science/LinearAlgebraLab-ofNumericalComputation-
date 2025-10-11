# LAL manual

## 数学命令

### 矩阵创建与操作

- 关键词预览：

```
1.eye        % 单位矩阵 **
2.zeros      % 零矩阵 **
3.ones       % 全1矩阵 **
4.rand       % 均匀分布随机矩阵 *
5.randn      % 正态分布随机矩阵
6.diag       % 对角矩阵或获取对角线元素 **
7.linspace   % 线性间隔向量 **
8.logspace   % 对数间隔向量
9.meshgrid   % 生成网格坐标
10.repmat    % 复制和平铺矩阵 *
11.reshape   % 改变矩阵形状 *
12.cat       % 连接数组 *
13.horzcat   % 水平连接 *
14.vertcat   % 垂直连接 *
15.blkdiag   % 块对角矩阵
16.tril      % 下三角部分 *
17.triu      % 上三角部分 *
18.flip      % 翻转矩阵 *
```

### 矩阵基本信息

- 关键词预览：

```
1.size       % 矩阵维度 **
2.length     % 最大维度长度 **
3.ndims      % 维度数量
4.numel      % 元素个数 **
5.isscalar   % 是否为标量 **
6.isvector   % 是否为向量 **
7.ismatrix   % 是否为矩阵 **
8.isempty    % 是否为空矩阵 **
9.isequal    % 判断矩阵是否相等
10.isnan     % 检查NaN值
11.isinf     % 检查无穷大
12.isfinite  % 检查有限值
```

### 矩阵运算

- 关键词预览：

```
1.transpose  % 转置 **
2.ctranspose % 共轭转置 ('运算符) **
3.plus       % 加法 (+) **
4.minus      % 减法 (-) **
5.mtimes     % 矩阵乘法 (*) **
6.times      % 逐元素乘法 (.*) **
7.mpower     % 矩阵幂 (^) **
8.power      % 逐元素幂 (.^) **
9.mldivide   % 左除 (\) **
10.mrdivide  % 右除 (/) **
11.divide    % 逐元素除 **
13.cross     % 向量叉积 **
14.dot       % 向量点积 **
15.sum       % 元素求和 **
16.mean      % 平均值 **
17.max       % 最大值 **
18.min       % 最小值 **
19.sort      % 排序
20.kron      % Kronecker 张量积
```

### 矩阵分解

- 关键词预览：

```
1.lu         % LU 分解 **
2.qr         % QR 分解 **
3.chol       % Cholesky 分解 **
4.svd        % 奇异值分解 **
5.eig        % 特征值分解 **
6.schur      % Schur 分解
7.hess       % Hessenberg 形式
8.qz         % QZ 分解（广义特征值）
```

### 线性方程组求解

- 关键词预览：

```
1.linsolve   % 线性方程组求解 **
2.inv        % 矩阵求逆 **
3.pinv       % 伪逆（Moore-Penrose） **
4.lscov      % 最小二乘解（已知协方差）
5.rref       % 简化行阶梯形式 **
6.null       % 零空间基 *
7.orth       % 正交化 *
```

### 特征值与奇异值

- 关键词预览：

```
1.poly       % 特征多项式
2.condeig    % 特征值条件数
3.balance    % 矩阵平衡（改善特征值计算）
```

### 矩阵分析与条件数

- 关键词预览：

```
1.det        % 行列式 **
2.rank       % 矩阵的秩 **
3.trace      % 矩阵的迹 **
4.norm       % 矩阵或向量范数 **
5.cond       % 矩阵条件数 **
6.condest    % 1-范数条件数估计
7.rcond      % 条件数倒数估计
```

### 矩阵函数

- 关键词预览：

```
1.expm       % 矩阵指数
2.logm       % 矩阵对数
3.sqrtm      % 矩阵平方根
4.funm       % 一般矩阵函数
```

### 特殊矩阵

- 关键词预览：

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
```

### 迭代法和预处理

- 关键词预览：

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
```


## 基本命令



## 常量宏